#include <sdsl/rmq_support.hpp> // include header for range minimum queries
#include "sdsl/memory_management.hpp"
#include <algorithm>
#include <cmath>
#include <getopt.h> 
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>
 
#include "../rmq/includes/RMQRMM64.h"
#include "../succinct/cartesian_tree.hpp"
#include "../succinct/mapper.hpp"
#include "hardware_event.h"

#define MILLI 1000
#define MICRO 1000000


using namespace std;
using namespace sdsl;
 
using ll = long long;
using query = std::pair<ll,ll>;
using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

int rmq_type;
HighResClockTimepoint s, e;

bool count_cache_misses = false;
bool compare_sdsl = false;
HardwareEvent hw_event;


struct query_stats {
    size_t N;
    double construction_time;
    double bits_per_element;
    std::vector<query> q;
    std::vector<double> q_time;
    std::vector<double> miss_ratio;
    std::vector<ll> cache_miss;
    std::vector<ll> cache_references;
    string algo;
    
    query_stats(string& algo) : algo(algo) { }
    
    void addQueryResult(query& qu, double time, double cache_miss_ratio, ll cache_misses, ll cache_ref) {
        q.push_back(qu);
        q_time.push_back(time);
        miss_ratio.push_back(cache_miss_ratio);
        cache_miss.push_back(cache_misses);
        cache_references.push_back(cache_ref);
    }
    
    
    void printQueryStats() {
        for(size_t i = 0; i < q.size(); ++i) {
            ll range = q[i].second - q[i].first + 1;
            printf("QUERY_RESULT Algo=%s N=%zu Range=%lld Time=%f MissRatio=%f CacheMisses=%lld CacheReferences=%lld\n", 
                    algo.c_str(), N, range, q_time[i], miss_ratio[i], cache_miss[i], cache_references[i]);
        }
    }
    
};

struct construction_stats {
    size_t N;
    double construction_time;
    double bits_per_element;
    string algo;
    
    construction_stats(string& algo) : algo(algo) { }
    
  
    void addConstructionResult(size_t n, double c_time, double bpe) {
        N = n;
        construction_time = c_time;
        bits_per_element = bpe; 
    }
    
    void printConstructionStats() {
        printf("CONSTRUCTION_RESULT Algo=%s N=%zu ConstructTime=%f BitsPerElement=%f\n",algo.c_str(),N,construction_time,bits_per_element);
    }
    
};

 
inline HighResClockTimepoint time() {
    return std::chrono::high_resolution_clock::now();
}

double milliseconds() {
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count()*MILLI;
}

double microseconds() {
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count()*MICRO;
}

template<class RMQ>
class RMQExperiment {
    
public:
    RMQExperiment(string& algo, int_vector<> *seq , vector<vector<query>>& qry) 
                 : algo(algo), q_stats(qry.size(), query_stats(algo)), c_stats(algo) { 
        s = time();
        RMQ rmq(seq);
        e = time();
        
        for(size_t i = 0; i < qry.size(); ++i) {
            c_stats.addConstructionResult(seq->size(),milliseconds(),
                                          8.0*(static_cast<double>(size_in_bytes(rmq))/static_cast<double>(seq->size())));
        }
        c_stats.printConstructionStats();
        write_structure<HTML_FORMAT>(rmq, "HTML/"+algo+".html");
        
        
        ofstream out("benchmark/"+algo+".txt");
        for(int i = 0; i < qry.size(); ++i) {
            q_stats[i].N = seq->size();
            for(int j = 0; j < qry[i].size(); ++j) {
                ll i1 = qry[i][j].first, i2 = qry[i][j].second;
                
                if(count_cache_misses) {
                    bool success = hw_event.start(PERF_COUNT_HW_CACHE_MISSES); 
                    if(!success) {
                        perror("perf_event_open");
                        exit(-1);   
                    }
                }
            
                s = time();
                auto res = rmq(i1,i2);
                e = time();
                
                double miss_ratio = 0;
                if(count_cache_misses) {
                    hw_event.stop();
                    miss_ratio = hw_event.getCacheMissRatio();
                }
                
                out << res << "\n";
                q_stats[i].addQueryResult(qry[i][j],microseconds(),miss_ratio,hw_event.getCacheMisses(),hw_event.getCacheReferences());
            }
            q_stats[i].printQueryStats(); 
        }
        
    }
    
private:
    string& algo;
    vector<query_stats> q_stats;
    construction_stats c_stats;
};


void executeRMQFerrada(long int *A, size_t N, vector<vector<query>>& qry) {
    string algo = "RMQ_FERRADA";
    vector<query_stats> q_stats(qry.size(),query_stats(algo));
    construction_stats c_stats(algo);
    
    s = time();
    RMQRMM64 rmq(A,N);
    e = time();
    
    c_stats.addConstructionResult(N,milliseconds(),
                                  8.0*(static_cast<double>(rmq.getSize())/static_cast<double>(N)));
    c_stats.printConstructionStats();
    
    for(int i = 0; i < qry.size(); ++i) {
        q_stats[i].N = N;
        for(int j = 0; j < qry[i].size(); ++j) {
            ll i1 = qry[i][j].first, i2 = qry[i][j].second;
            if(i1 > ULONG_MAX || i2 > ULONG_MAX) continue;
           
            if(count_cache_misses) {
                bool success = hw_event.start(PERF_COUNT_HW_CACHE_MISSES); 
                if(!success) {
                    perror("perf_event_open");
                    exit(-1);   
                }
            }
            
            s = time();
            auto res = rmq.queryRMQ(i1,i2);
            e = time();
            
            double miss_ratio = 0;
            if(count_cache_misses) {
                hw_event.stop();
                miss_ratio = hw_event.getCacheMissRatio();
            }
            
	    q_stats[i].addQueryResult(qry[i][j],microseconds(),miss_ratio,hw_event.getCacheMisses(),hw_event.getCacheReferences());
        }
        q_stats[i].printQueryStats();
    }
    
}

void executeRMQSuccinct(std::vector<long long>& A, size_t N, vector<vector<query>>& qry) {
    string algo = "RMQ_SUCCINCT";
    vector<query_stats> q_stats(qry.size(),query_stats(algo));
    construction_stats c_stats(algo);
    
    s = time();
    succinct::cartesian_tree rmq(A);
    e = time();
    
    double size_succinct = 8.0*(static_cast<double>(succinct::mapper::size_of<succinct::cartesian_tree>(rmq))/static_cast<double>(N));
    
    c_stats.addConstructionResult(N,milliseconds(),size_succinct);
    c_stats.printConstructionStats();
     
    for(int i = 0; i < qry.size(); ++i) {
        q_stats[i].N = N;
        for(int j = 0; j < qry[i].size(); ++j) {
            uint64_t i1 = qry[i][j].first, i2 = qry[i][j].second;
            if(i1 > ULONG_MAX || i2 > ULONG_MAX) continue;
            
            if(count_cache_misses) {
                bool success = hw_event.start(PERF_COUNT_HW_CACHE_MISSES); 
                if(!success) {
                    perror("perf_event_open");
                    exit(-1);   
                }
            }
            
            s = time();
            auto res = rmq.rmq(i1,i2);
            e = time();

            double miss_ratio = 0;
            if(count_cache_misses) {
                hw_event.stop();
                miss_ratio = hw_event.getCacheMissRatio();
            }
  
            q_stats[i].addQueryResult(qry[i][j],microseconds(),miss_ratio,hw_event.getCacheMisses(),hw_event.getCacheReferences());
        }
        q_stats[i].printQueryStats();
    }
    
}
    

int main(int argc, char *argv[]) {
    
    
    ios::sync_with_stdio(false);
    
    std::string in_file = std::string(argv[1]);
    ifstream is(in_file);
    printf("Read Input Sequence...\n");
    size_t N; is >> N;
    size_t a,b; is >> a >> b;
    int_vector<> A(N); 
    for(size_t i = 0; i < N; ++i) {
        ll x; is >> x;
        A[i] = x;
        if(A[i] != x) return -1;
    }
    is.close();
    
     
    printf("Read Query Files...\n");
    int num_qry = atoi(argv[2]);
    vector<vector<query>> qv(num_qry);
    for(int i = 0; i < num_qry; ++i) {
        string query_file = std::string(argv[3+i]);
        ifstream qis(query_file);
        int q; qis >> q;
        for(int j = 0; j < q; ++j) {
            query qu = make_pair(0,0); qis >> qu.first >> qu.second;
            qv[i].push_back(qu);
        }
        qis.close(); 
    }
    
    if(argc > 3+num_qry) {
        count_cache_misses = atoi(argv[3+num_qry]);
        if(argc > 3+num_qry + 1) {
            compare_sdsl = atoi(argv[3+num_qry+1]);
        }
    }
   
    if(compare_sdsl) {

        {
            string algo = "RMQ_SDSL_REC_4096_1"; 
            RMQExperiment<rmq_succinct_rec_new<true, 4096,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_2048_1"; 
            RMQExperiment<rmq_succinct_rec_new<true, 2048,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_1024_1"; 
            RMQExperiment<rmq_succinct_rec_new<true, 1024,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_4096_2"; 
            RMQExperiment<rmq_succinct_rec_new<true, 4096,128,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_2048_2"; 
            RMQExperiment<rmq_succinct_rec_new<true, 2048,128,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_1024_2"; 
            RMQExperiment<rmq_succinct_rec_new<true, 1024,128,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_4096_3"; 
            RMQExperiment<rmq_succinct_rec_new<true, 4096,128,64,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_2048_3"; 
            RMQExperiment<rmq_succinct_rec_new<true, 2048,128,64,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_REC_1024_3"; 
            RMQExperiment<rmq_succinct_rec_new<true, 1024,128,64,0>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_SCT";
            RMQExperiment<rmq_succinct_sct<>> rmq(algo,&A,qv);
        } 
        
    }
    else {
        
        {
            string algo = "RMQ_SDSL_REC_NEW_1024_2"; 
            RMQExperiment<rmq_succinct_rec_new<true, 1024,128,0>> rmq(algo,&A,qv);
        }
        
        
        {
            string algo = "RMQ_SDSL_REC_OLD_1024_2"; 
            RMQExperiment<rmq_succinct_rec<>> rmq(algo,&A,qv);
        }
        
        {
            string algo = "RMQ_SDSL_SCT";
            RMQExperiment<rmq_succinct_sct<>> rmq(algo,&A,qv);
        } 
        
        
        long int *B = new long int[N];
        for(size_t i = 0; i < N; ++i) {
            B[i] = A[i];
            if(B[i] != A[i]) return -1;
        }
        memory_manager::clear(A);
        
        
        {
            executeRMQFerrada(B,N,qv);
        } 
        
        if(N < std::numeric_limits<int>::max()) {
            std::vector<long long> C(N);
            for(size_t i = 0; i < N; ++i) {
                C[i] = B[i];
                if(C[i] != B[i]) return -1;
            }
            delete [] B;
            
            {
                executeRMQSuccinct(C,N,qv);
            }
        }
        else {
            delete [] B;
        }
    }
    
    
}
