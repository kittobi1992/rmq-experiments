#include <algorithm>
#include <getopt.h> 
#include <random>
#include <fstream>
#include <iostream>

using ll = long long;

struct config {
    ll N;
    ll q; 
    ll r;
    std::string ofile;
    
    void printConfig() {
        printf("=============\nConfiguration \n=============\n");
        printf("Sequence length: %lld\n", N);
        printf("Num Queries: %lld\n", q);
        printf("Query Range Size: %lld\n", r);
        printf("Output file: %s\n", ofile.c_str());
        printf("---------------------------\n");
    }
};

void
usage(char* program)
{
    printf("%s \n", program);
    printf("where\n");
    printf("  -N    : size of the sequence\n");
    printf("  -q    : generating q queries for sequence.\n");
    printf("  -r    : range of the query interval \n");
    printf("  -f    : output file for query\n");
    exit(EXIT_FAILURE);
};

config parse_args(int argc, char* const argv[]) {
    config con;
    int op;
    while ((op = getopt(argc, argv, "n:q:r:f:")) != -1) {
        switch (op) {
            case 'n': con.N = std::stoull(std::string(optarg));
            break;
            case 'q': con.q = std::stoull(std::string(optarg));
            break;
            case 'r': con.r = std::stoull(std::string(optarg));
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

int main(int argc, char* const argv[]) {
    
    
    std::ios::sync_with_stdio(false);
    
    config con = parse_args(argc,argv);
    con.printConfig();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<ll> dis(0, con.N-con.r);
    printf("Generating random queries...\n");
    std::ofstream os;
    os.open(con.ofile);
    os << con.q << "\n";
    for(size_t i = 0; i < con.q; ++i) {
        ll i1 = dis(gen), i2 = i1 + con.r - 1;
        os << i1 << " " << i2 << "\n";
    }
    os.close();
    printf("Queries are written to %s\n", con.ofile.c_str());
    
    printf("Finish!\n");
    
    
    return 0;
}
