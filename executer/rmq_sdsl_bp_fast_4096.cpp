#include <sdsl/rmq_support.hpp> // include header for range minimum queries
#include <algorithm>
#include <cmath>
#include <getopt.h> 
#include <fstream>
#include <iostream>
#include <chrono>

#define MILLI 1000
#define MICRO 1000000


using namespace std;
using namespace sdsl;

using query = std::pair<int,int>;
using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

int rmq_type;
HighResClockTimepoint s, e;

struct stats {
    double construction_time;
    double bits_per_element;
    std::vector<double> q_time;
    
    void addQueryResult(double time) {
        q_time.push_back(time);
    }
    
    
    void printStats() {
        printf("RESULT Algo=RMQ_SDSL_BP_FAST_4096 ConstructTime=%f",construction_time);
        printf(" BitsPerElement=%f",bits_per_element);
        sort(q_time.begin(),q_time.end());
        size_t N = q_time.size();
        double avg_time = 0.0;
        for(size_t i = 0; i < N; ++i) avg_time += q_time[i];
        avg_time /= N;
        double sigma = 0.0;
        for(size_t i = 0; i < N; ++i) sigma += (q_time[i]-avg_time)*(q_time[i]-avg_time);
        sigma = std::sqrt(sigma/(N-1));
        printf(" AvgQueryTime=%f",avg_time);
        printf(" MedianQueryTime=%f",q_time[N/2]);
        printf(" MinQueryTime=%f",q_time[0]);
        printf(" MaxQueryTime=%f",q_time[N-1]);
        printf(" StandardDerivation=%f\n",sigma);
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

int main(int argc, char *argv[]) {
    
    
    ios::sync_with_stdio(false);
    
    std::string in_file = std::string(argv[1]);
    ifstream is(in_file);
    printf("Read Input Sequence...\n");
    long long N; is >> N;
    int_vector<> A(N);
    for(int i = 0; i < N; ++i) {
        int x; is >> x;
        A[i] = x;
    }
    is.close();
    
    stats rmq_stats;
    
    printf("Constructing RMQ...\n");
    s = time();
    rmq_succinct_bp_fast<4096> rmq(&A);
    //rmq_succinct_sct<> rmq2(&A);
    e = time();
    rmq_stats.construction_time = milliseconds();
    rmq_stats.bits_per_element = static_cast<double>(size_in_bytes(rmq)*8)/static_cast<double>(N);
    
    //rmq.print_bp();
    printf("Read Query File...\n");
    std::vector<query> qv;
    std::string query_file = argv[2];
    ifstream qis(query_file);
    int q; qis >> q;
    for(int i = 0; i < q; ++i) {
        query qu = make_pair(0,0); qis >> qu.first >> qu.second;
        qv.push_back(qu);
    }
    qis.close();
    
    for(int i = 0; i < q; ++i) {
        s = time();
        //std::cout << qv[i].first << " - " << qv[i].second << std::endl;
        auto res = rmq(qv[i].first,qv[i].second);
        //auto res2 = rmq2(qv[i].first, qv[i].second);
        //std::cout << res << " == " << res2 << " - " << A[res] << " == " << A[res2] << std::endl;
        //if(res != res2) exit(-1);
        //std::cout << "Query RMQ_A["<<qv[i].first<<","<<qv[i].second<<"] = " <<res << " - A["<<res<<"] = " << A[res] << std::endl;
        e = time();
        rmq_stats.addQueryResult(microseconds());
    }
    
    //write_structure<HTML_FORMAT>(rmq, "rmq_4096.html");
    
    rmq_stats.printStats();
}