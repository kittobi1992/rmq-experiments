CC=g++
CFLAGS=-std=c++11 -O3
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_SUFFIX=-lsdsl -ldivsufsort -ldivsufsort64
FERRADA_LIB=rmq/rmqrmmBP.a

all: Experiments

Ferrada: rmq/RMQRMM64.cpp
	 cd rmq && $(MAKE)
	 
Succinct: succinct/cartesian_tree.hpp
	  cd succinct && cmake . && $(MAKE)
	  
Generators: generators/gen_sequence.cpp generators/gen_query.cpp
	    $(CC) $(CFLAGS) generators/gen_sequence.cpp -o generators/gen_sequence.o
	    $(CC) $(CFLAGS) generators/gen_query.cpp -o generators/gen_query.o

Experiments: executer/rmq_experiment.cpp
	     $(CC) $(CFLAGS) $(SDSL_PREFIX) executer/rmq_experiment.cpp -o executer/rmq_experiment.o $(SDSL_SUFFIX) $(FERRADA_LIB)
	     
SDSL: 
	bash -x ../sdsl-lite/build/build.sh