#include <sdsl/rmq_support.hpp> // include header for range minimum queries
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
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


#include "../rmq/includes/RMQRMM64.h"
#include "../succinct/cartesian_tree.hpp"

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

template<class RMQ>
void traverseSuffixTree(RMQ& rmq, int_vector<> lcp) {
    size_t N = rmq.size();
    stack<state> s; s.emplace(0,N-1,0);
    
    while(!s.empty()) {
        state cur = s.top(); s.pop();
//                    if(cur.i != cur.j) cout << "Internal Node: (" << cur.i << "," << cur.j << ") - " << cur.l << endl;
        if(cur.i == cur.j) {
//                           cout << "Leaf: " << cur.i << endl;
            continue;
        }
        
        ll cur_i = cur.i;
        while(cur_i < cur.j) {
            size_t min_i = rmq(cur_i+1,cur.j);
//                           cout << cur_i << " " << cur.j << " " << min_i << endl;
            ll ii = cur_i; ll jj = cur.j-1;
            if(lcp[min_i] == cur.l && min_i < cur.j) jj = min_i-1;
            else if(lcp[min_i] != cur.l) jj = cur.j;
            if(ii+1 <= jj) {
                size_t l_idx = rmq(ii+1,jj);
                s.emplace(ii,jj,lcp[l_idx]);
            }
            else if(ii == jj) s.emplace(ii,ii,lcp[ii]);
            cur_i = jj+1;
        }
        
    }
}

void traverseSuffixTreeSuccinct(succinct::cartesian_tree& rmq, int_vector<> lcp) {
    size_t N = rmq.size();
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
            size_t min_i = rmq.rmq(cur_i+1,cur.j);
            //               cout << cur_i << " " << cur.j << " " << min_i << endl;
            ll ii = cur_i; ll jj = cur.j-1;
            if(lcp[min_i] == cur.l && min_i < cur.j) jj = min_i-1;
            else if(lcp[min_i] != cur.l) jj = cur.j;
            if(ii+1 <= jj) {
                size_t l_idx = rmq.rmq(ii+1,jj);
                s.emplace(ii,jj,lcp[l_idx]);
            }
            else if(ii == jj) s.emplace(ii,ii,lcp[ii]);
            cur_i = jj+1;
        }
        
    }
}

void traverseSuffixTreeFerrada(RMQRMM64& rmq, int_vector<> lcp) {
    size_t N = lcp.size();
    stack<state> s; s.emplace(0,N-1,0);
    
    while(!s.empty()) {
        state cur = s.top(); s.pop();
//         if(cur.i != cur.j) cout << "Internal Node: (" << cur.i << "," << cur.j << ") - " << cur.l << endl;
        if(cur.i == cur.j) {
//             cout << "Leaf: " << cur.i << endl;
            continue;
        }
        
        ll cur_i = cur.i;
        while(cur_i < cur.j) {
            size_t min_i = N-rmq.queryRMQ(N-cur.j-1,N-(cur_i+1)-1)-1;
//                           cout << cur_i << " " << cur.j << " " << min_i << endl;
            ll ii = cur_i; ll jj = cur.j-1;
            if(lcp[min_i] == cur.l && min_i < cur.j) jj = min_i-1;
            else if(lcp[min_i] != cur.l) jj = cur.j;
            if(ii+1 <= jj) {
                size_t l_idx = N-rmq.queryRMQ(N-jj-1,N-(ii+1)-1)-1;
                s.emplace(ii,jj,lcp[l_idx]);
            }
            else if(ii == jj) s.emplace(ii,ii,lcp[ii]);
            cur_i = jj+1;
        }
        
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
    string algo2 = "RMQ_SUCCINCT_REC_OLD_1024";
    string algo3 = "RMQ_SUCCINCT_SCT";
    string algo4 = "RMQ_FERRADA";
    string algo5 = "RMQ_SUCCINCT";
    
    {
        rmq_succinct_rec<1024> rmq(&lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo1 << "..." << endl;
        s = time();
        traverseSuffixTree<rmq_succinct_rec<1024>>(rmq,lcp);
        e = time();
        double percentage_avoided = (static_cast<double>(rmq.num_avoided_selects)/static_cast<double>(rmq.num_queries));
        std::cout << rmq.num_avoided_selects << " out of " << rmq.num_queries << " queries (" << percentage_avoided << "%) avoids second select" << std::endl;
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo1 << " Time=" << t << std::endl;
    }
    
    
    {
        rmq_succinct_rec_old<1024> rmq(&lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo2 << "..." << endl;
        s = time();
        traverseSuffixTree<rmq_succinct_rec_old<1024>>(rmq,lcp);
        e = time();
        double percentage_avoided = (static_cast<double>(rmq.num_avoided_selects)/static_cast<double>(rmq.num_queries));
        std::cout << rmq.num_avoided_selects << " out of " << rmq.num_queries << " queries (" << percentage_avoided << "%) avoids second select" << std::endl;
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo2 << " Time=" << t << std::endl;
    }
    
    {
        rmq_succinct_sct<> rmq(&lcp);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo3 << "..." << endl;
        s = time();
        traverseSuffixTree<rmq_succinct_sct<>>(rmq,lcp);
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo3 << " Time=" << t << std::endl;
    }
    
    {
        size_t N = lcp.size();
        long int *B = new long int[N];
        for(size_t i = 0; i < N; ++i) {
            B[N-i-1] = lcp[i];
        }
        RMQRMM64 rmq(B,N);
        delete [] B;
        cout << "Start Suffix-Tree Traversion for RMQ " << algo4 << "..." << endl;
        s = time();
        traverseSuffixTreeFerrada(rmq,lcp);
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo4 << " Time=" << t << std::endl;
    }
    
    if(test_id != "proteins") {
        size_t N = lcp.size();
        std::vector<long long> C(N);
        for(size_t i = 0; i < N; ++i) {
            C[i] = lcp[i];
        }
        succinct::cartesian_tree rmq(C);
        cout << "Start Suffix-Tree Traversion for RMQ " << algo5 << "..." << endl;
        s = time();
        traverseSuffixTreeSuccinct(rmq,lcp);
        e = time();
        double t = seconds();
        std::cout << "LCP_RESULT Benchmark=" << test_id << " Algo=" << algo5 << " Time=" << t << std::endl;
    }
    

    
    
}
