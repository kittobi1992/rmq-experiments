#include <sdsl/rmq_support.hpp> // include header for range minimum queries
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/construct_lcp.hpp>
#include <sdsl/construct_bwt.hpp>
#include <algorithm>
#include <cmath>
#include <getopt.h> 
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>
#include <stack>

#define x first
#define y second


using namespace std;
using namespace sdsl;

typedef map<string, void (*)(cache_config&)> tMSFP;

using ll = long long;
using ival = pair<ll,ll>;

struct state {
  ll i,j,l;
  state(ll i, ll j, ll l) : i(i), j(j), l(l) { }
};

using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

HighResClockTimepoint s, e;

inline HighResClockTimepoint time() {
    return std::chrono::high_resolution_clock::now();
}

double seconds() {
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count();
}

template<class RMQ>
class LCPExperiment {
    
public:
    LCPExperiment(string& algo, int_vector<>& lcp) 
    : _algo(algo), _rmq(&lcp), _lcp(lcp) { }

    void traverseSuffixTree() {
        size_t N = _rmq.size();
        stack<state> s; s.emplace(0,N-1,0);
        
        while(!s.empty()) {
            state cur = s.top(); s.pop();
//             if(cur.i != cur.j) cout << "Internal Node: (" << cur.i << "," << cur.j << ") - " << cur.l << endl;
            if(cur.i == cur.j) {
//               cout << "Leaf: " << cur.i << endl;
              continue;
            }
            
            ll cur_i = cur.i;
            while(cur_i < cur.j) {
              size_t min_i = _rmq(cur_i+1,cur.j);
//               cout << cur_i << " " << cur.j << " " << min_i << endl;
              ll ii = cur_i; ll jj = cur.j-1;
              if(_lcp[min_i] == cur.l && min_i < cur.j) jj = min_i-1;
              else if(_lcp[min_i] != cur.l) jj = cur.j;
              if(ii+1 <= jj) {
                size_t l_idx = _rmq(ii+1,jj);
                s.emplace(ii,jj,_lcp[l_idx]);
              }
              else if(ii == jj) s.emplace(ii,ii,_lcp[ii]);
              cur_i = jj+1;
            }
            
        }
    }
    
private:
    string& _algo;
    RMQ _rmq;
    int_vector<>& _lcp;
};


void construct_lcp(cache_config& test_config, string& test_file) {
    tMSFP lcp_function;
    lcp_function["bwt_based"] = &construct_lcp_bwt_based;
    lcp_function["bwt_based2"] = &construct_lcp_bwt_based2;
    lcp_function["PHI"] = &construct_lcp_PHI<8>;
    lcp_function["semi_extern_PHI"] = &construct_lcp_semi_extern_PHI;
    lcp_function["go"] = &construct_lcp_go;
    lcp_function["goPHI"] = &construct_lcp_goPHI;
    
    {
        cout << "Load text..." << endl;
        int_vector<8> text;
        load_vector_from_file(text,test_file,1);
        append_zero_symbol(text);
        store_to_cache(text, conf::KEY_TEXT, test_config);
        
        cout << "Construct Suffix Array..." << endl;
        int_vector<> sa(text.size(), 0,bits::hi(text.size())+1);
        algorithm::calculate_sa((const unsigned char*)text.data(), text.size(), sa);
        store_to_cache(sa,conf::KEY_SA,test_config);
    }
    
    {
        cout << "Construct LCP Array..." << endl;
        construct_lcp_PHI<8>(test_config);
    }
}

int main(int argc, char *argv[]) {
    
    string test_file, temp_dir, test_id;
    
    test_file   = argv[1];
    temp_dir    = argv[2];
    test_id     = test_file.substr(test_file.find_last_of("/\\") + 1);
    
    cache_config test_config = cache_config(false, temp_dir, test_id);
    
    string lcp_file = cache_file_name(conf::KEY_LCP, test_config);
    int_vector<> lcp;
    if(!load_from_file(lcp,lcp_file)) {
        construct_lcp(test_config,test_file);
        load_from_file(lcp, lcp_file);
    }
    
    string algo1 = "RMQ_SUCCINCT_REC_1024";
    string algo2 = "RMQ_SUCCINCT_BP_FAST_1024";
    string algo3 = "RMQ_SUCCINCT_SCT";
    
    {
        LCPExperiment<rmq_succinct_rec<1024>> rmq(algo1,lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo1 << "..." << endl;
        s = time();
        rmq.traverseSuffixTree();
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo1 << " Time=" << t << std::endl;
    }
    
    {
        LCPExperiment<rmq_succinct_sct<>> rmq(algo3,lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo3 << "..." << endl;
        s = time();
        rmq.traverseSuffixTree();
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo3 << " Time=" << t << std::endl;
    }
    

    
    /*{
        LCPExperiment<rmq_succinct_bp_fast<1024>> rmq(algo2,lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo2 << "..." << endl;
        s = time();
        rmq.traverseSuffixTree();
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo2 << " Time=" << t << std::endl;
    }*/

    
    
}
