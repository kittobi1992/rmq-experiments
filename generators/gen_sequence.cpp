#include <algorithm>
#include <getopt.h> 
#include <random>
#include <fstream>
#include <iostream>
 
using ll = long long;
ll min_value = 1LL;
ll max_value = static_cast<ll>(std::numeric_limits<ll>::max());

struct config {
    ll N;
    ll a; 
    ll b;//Generate numbers in interval [a..b]
    int pseudo_sorted;//0: random values | 1: pseudo sorted increasing | 2: pseudo sorted decreasing
    ll delta;
    std::string ofile;
    
    config() : N(10), a(0), b(10), pseudo_sorted(0), delta(0), ofile("experiments/1.seq") { }
    
    void printConfig() {
        printf("=============\nConfiguration \n=============\n");
        printf("Sequence length: %lld\n", N);
        printf("Generating numbers in interval [%lld,%lld]\n", a,b);
        printf("Sequence type: %d\n", pseudo_sorted);
        printf("Delta: %lld\n", delta);
        printf("Output file: %s\n", ofile.c_str());
        printf("---------------------------\n");
    }
};

std::random_device rd;
std::mt19937 gen(rd());

void
usage(char* program)
{
    printf("%s \n", program);
    printf("where\n");
    printf("  -N    : size of the sequence which should be generated.\n");
    printf("  -a -b : generating numbers in interval [a,b].\n");
    printf("  -p    : 0: random values | 1: pseudo sorted increasing | 2: pseudo sorted decreasing.\n");
    printf("  -d    : delta for pseudo sorted sequence (default: 0).\n");
    printf("  -f    : generated sequence output file \n");
    exit(EXIT_FAILURE);
};

config parse_args(int argc, char* const argv[]) {
    config con;
    int op;
    while ((op = getopt(argc, argv, "n:a:b:f:p:d:")) != -1) {
        switch (op) {
            case 'n': con.N = std::stoull(std::string(optarg));
                      break;
            case 'a': con.a = std::stoull(std::string(optarg));
                      break;
            case 'b': con.b = std::stoull(std::string(optarg));
                      break;
            case 'p': con.pseudo_sorted = std::stoull(std::string(optarg));
                      break;
            case 'd': con.delta = std::stoull(std::string(optarg));
                      break;
            case 'f': con.ofile = optarg;
                      break;
            case '?':
            default:
                usage(argv[0]);
        }
    }
    return con;
}

void writeRandomSequence(config& con, std::ofstream& os) {
    printf("Starting generating random values in range [%lld,%lld]\n",con.a,con.b);
    std::uniform_int_distribution<ll> dis(con.a, con.b);
    for(ll i = 0; i < con.N; ++i) {
        os << std::min(std::max(dis(gen),min_value),max_value) << (i+1 == con.N ? "\n" : " ");
    }
}

void writePseudoSortedIncreasingSequence(config& con, std::ofstream& os) {
    printf("Starting generating pseudo increasing sequence with A[i] in [i-%lld,i+%lld]\n",con.delta,con.delta);
    std::uniform_int_distribution<ll> dis(-con.delta, con.delta);
    for(ll i = 0; i < con.N; ++i) {
        os << std::min(std::max((i+dis(gen)),min_value),max_value) << (i+1 == con.N ? "\n" : " ");
    }
}

void writePseudoSortedDecreasingSequence(config& con, std::ofstream& os) {
    printf("Starting generating pseudo decreasing sequence with A[i] in [%lld-i-%lld,%lld-i+%lld]\n",con.N,con.delta,con.N,con.delta);
    std::uniform_int_distribution<ll> dis(-con.delta, con.delta);
    for(ll i = 0; i < con.N; ++i) {
        os << std::min(std::max((con.N-i+dis(gen)),min_value),max_value) << (i+1 == con.N ? "\n" : " ");
    }
}

void writeEqualSequence(config& con, std::ofstream& os) {
    printf("Starting generating pseudo decreasing sequence with A[i] in [%lld-i-%lld,%lld-i+%lld]\n",con.N,con.delta,con.N,con.delta);
    std::uniform_int_distribution<ll> dis(con.a, con.b);
    ll x = dis(gen);
    for(ll i = 0; i < con.N; ++i) {
        os << std::min(std::max(x,min_value),max_value) << (i+1 == con.N ? "\n" : " ");
    }
}


int main(int argc, char* const argv[]) {
    
    
    std::ios::sync_with_stdio(false);
    
    config con = parse_args(argc,argv);
    con.printConfig();
	
    printf("Generating random sequence...\n");
    std::ofstream os;
    os.open(con.ofile);
    os << con.N << "\n";
    switch(con.pseudo_sorted) {
        case 0: writeRandomSequence(con,os);
                break;
        case 1: writePseudoSortedIncreasingSequence(con,os);
                break;
        case 2: writePseudoSortedDecreasingSequence(con,os);
                break;
        case 3: writeEqualSequence(con,os);
                break;
        default: break;
    }
    os.close();
    printf("Sequence is written to %s\n", con.ofile.c_str());
    
    printf("Finish!\n");
    

	return 0;
}
