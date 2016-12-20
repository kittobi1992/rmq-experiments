CC=g++
CFLAGS=-std=c++11 -O3
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_SUFFIX=-lsdsl -ldivsufsort -ldivsufsort64
FERRADA_LIB=rmq/rmqrmmBP.a
SUCCINCT_LIB=succinct/libsuccinct.a

all: experiments

experiments: generators/gen_sequence.o generators/gen_query.o executer/rmq_experiment.o

rmq/RMQRMM64.o: rmq/RMQRMM64.cpp
	        cd rmq && $(MAKE)

succinct/libsuccinct.a: succinct/cartesian_tree.hpp
	                cd succinct && cmake . && $(MAKE)
	  
generators/gen_sequence.o: generators/gen_sequence.cpp
		           $(CC) $(CFLAGS) generators/gen_sequence.cpp -o generators/gen_sequence.o
		           
generators/gen_query.o: generators/gen_query.cpp
			$(CC) $(CFLAGS) generators/gen_query.cpp -o generators/gen_query.o

executer/rmq_experiment.o: executer/rmq_experiment.cpp rmq/RMQRMM64.o succinct/libsuccinct.a ../sdsl-lite/build/lib/libsdsl.a
	                   $(CC) $(CFLAGS) $(SDSL_PREFIX) executer/rmq_experiment.cpp -o executer/rmq_experiment.o $(SDSL_SUFFIX) $(FERRADA_LIB) $(SUCCINCT_LIB)
	     
../sdsl-lite/build/lib/libsdsl.a:   $(wildcard ../sdsl-lite/include/sdsl/*)
				    rm -f $@
				    bash -x ../sdsl-lite/build/build.sh
