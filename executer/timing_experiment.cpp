#include <sdsl/rmq_support.hpp> // include header for range minimum queries
#include "sdsl/memory_management.hpp"
#include <algorithm>
#include <cmath>
#include <getopt.h> 
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>

#define MILLI 1000
#define MICRO 1000000


using namespace std;
using namespace sdsl;

using ll = long long;
using query = std::pair<ll,ll>;




template<class RMQ>
class RMQExperiment {
    
public:
    RMQExperiment(string& algo, int_vector<> *seq , vector<vector<query>>& qry) 
                  : algo(algo) { 
        RMQ rmq(seq);
        write_structure<HTML_FORMAT>(rmq, "HTML/"+algo+".html");
        
        ofstream out("benchmark/"+algo+".txt");
        for(int i = 0; i < qry.size(); ++i) {
            for(int j = 0; j < qry[i].size(); ++j) {
                ll i1 = qry[i][j].first, i2 = qry[i][j].second;
                auto res = rmq(i1,i2);
            }
        }
        
    }
    
private:
    string& algo;
};



    

int main(int argc, char *argv[]) {
    
    
    ios::sync_with_stdio(false);
    
    std::string in_file = std::string(argv[1]);
    ifstream is(in_file);
    printf("Read Input Sequence...\n");
    size_t N; is >> N;
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
    
    string algo = "RMQ_SUCCINCT_REC_1024";
    {
        RMQExperiment<rmq_succinct_rec<1024>> rmq6(algo,&A,qv);
    }
    memory_manager::clear(A);
    
    
}
