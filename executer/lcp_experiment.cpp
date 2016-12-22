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
#define MILLI 1000
#define MICRO 1000000


using namespace std;
using namespace sdsl;

typedef map<string, void (*)(cache_config&)> tMSFP;

using ll = long long;
using ival = pair<ll,ll>;

template<class RMQ>
class LCPExperiment {
    
public:
    LCPExperiment(string& algo, int_vector<>& lcp) 
    : _algo(algo), _rmq(&lcp), _lcp(lcp) { 
        
        for(size_t i = 0; i < _lcp.size(); i++) cout << _lcp[i] << " ";
        cout << endl;
    }

    void traverseSuffixTree() {
        size_t N = _rmq.size();
        stack<ival> s; s.push(make_pair(0,N));
        
        while(!s.empty()) {
            ival cur = s.top(); s.pop();
            if(cur.x != cur.y-1) cout << "Internal Node: (" << cur.x << "," << cur.y-1 << ")" << endl;
            else cout << "Leaf: " << cur.x << endl;
            if(cur.y == cur.x) continue;
            else if(cur.y - cur.x == 1) continue;
            
            size_t cur_x = cur.x;
            while(cur_x < cur.y-1) {
                if(cur_x+1 > cur.y-1) break; 
                size_t min_i = _rmq(cur_x+1,cur.y-1);
                if(min_i == cur.y-1 && _lcp[cur_x] < _lcp[min_i]) min_i = cur.y;
//                 cout << "Rejected " << cur_x << " " << min_i-1 << endl;
                if(cur.x != cur_x || min_i != cur.y) {
                    s.push(make_pair(cur_x,min_i));
                } else {
                    s.push(make_pair(cur_x,cur_x+1));
                    s.push(make_pair(cur_x+1,cur.y));
                }
                cur_x = min_i;
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
        int_vector<8> text;
        load_vector_from_file(text,test_file,1);
        append_zero_symbol(text);
        store_to_cache(text, conf::KEY_TEXT, test_config);
        
        int_vector<> sa(text.size(), 0);
        algorithm::calculate_sa((const unsigned char*)text.data(), text.size(), sa);
        for(int i = 0; i < sa.size(); ++i) cout << sa[i] << " ";
        cout << endl;
        store_to_cache(sa,conf::KEY_SA,test_config);
    }
    
    {
        construct_lcp_PHI<8>(test_config);
    }
}

int main(int argc, char *argv[]) {
    
    string test_file, temp_dir, test_id;
    
    test_file   = argv[1];
    temp_dir    = argv[2];
    test_id     = 42;
    
    cache_config test_config = cache_config(false, temp_dir, test_id);
    construct_lcp(test_config,test_file);
    
    string lcp_file = cache_file_name(conf::KEY_LCP, test_config);
    int_vector<> lcp;
    load_from_file(lcp, lcp_file);
    append_zero_symbol(lcp);
    
    string algo1 = "RMQ_SUCCINCT_REC_1024";
    string algo2 = "RMQ_SUCCINCT_SCT";
    
    {
        LCPExperiment<rmq_succinct_rec<1024>> rmq(algo1,lcp);
        rmq.traverseSuffixTree();
    }
    
    {
        LCPExperiment<rmq_succinct_sct<>> rmq(algo2,lcp);
        rmq.traverseSuffixTree();
    }
    
    
}
