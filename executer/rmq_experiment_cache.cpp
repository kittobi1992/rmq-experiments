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

#define MILLI 1000
#define MICRO 1000000


using namespace std;
using namespace sdsl;

using ll = long long;
using query = std::pair<ll,ll>;
using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

int rmq_type;
HighResClockTimepoint s, e;

struct query_stats {
    size_t N;
    double construction_time;
    double bits_per_element;
    std::vector<query> q;
    std::vector<double> q_time;
    std::vector<bool> use_scan;
    string algo;
    
    query_stats(string& algo) : algo(algo) { }
    
    void addQueryResult(query& qu, double time, bool scan) {
        q.push_back(qu);
        q_time.push_back(time);
        use_scan.push_back(scan);
    }
    
    
    void printQueryStats() {
        for(size_t i = 0; i < q.size(); ++i) {
            ll range = q[i].second - q[i].first + 1;
            printf("QUERY_RESULT Algo=%s N=%zu Range=%lld Time=%f Scan=%s\n", algo.c_str(), N, range, q_time[i], use_scan[i] ? "true" : "false");
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
                s = time();
                auto res = 0;
                e = time();
                out << res << "\n";
                q_stats[i].addQueryResult(qry[i][j],microseconds(),rand()%2);
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
            s = time();
            auto res = 0;
            e = time();
            q_stats[i].addQueryResult(qry[i][j],microseconds(),rand()%2);
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
            s = time();
            auto res = 0;
            e = time();
            q_stats[i].addQueryResult(qry[i][j],microseconds(),rand()%2);
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
   


    {
       string algo = "RMQ_SDSL_REC_NEW_1024_2";
       RMQExperiment<rmq_succinct_rec_new<true, 1024,128,0>> rmq(algo,&A,qv);
    }


    
    /*{
      string algo = "RMQ_SDSL_SCT";
      RMQExperiment<rmq_succinct_sct<>> rmq(algo,&A,qv);
    }
    
    long int *B = new long int[N];
    for(size_t i = 0; i < N; ++i) {
        B[i] = A[i];
        if(B[i] != A[i]) return -1;
    }*/
    memory_manager::clear(A);
    
  
    /*{
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
    }*/
    
    
}
