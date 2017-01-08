#include <algorithm>
#include <cmath>
#include <getopt.h> 
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>

#include "../rmq/includes/RMQRMM64.h"

#define MILLI 1000
#define MICRO 1000000


using namespace std;

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
            auto res = rmq.queryRMQ(i1,i2);
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
    long int *B = new long int[N];
    for(size_t i = 0; i < N; ++i) {
        ll x; is >> x;
        B[i] = x;
        if(B[i] != x) return -1;
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
        executeRMQFerrada(B,N,qv);
    } 
    
    delete [] B;
    
    
}
