CC=g++
CFLAGS=-std=c++11 -O3
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_SUFFIX=-ldivsufsort -ldivsufsort64

all: Ferrada Succinct

Ferrada: rmq/RMQRMM64.cpp
	 cd rmq && $(MAKE)
	 
Succinct: succinct/cartesian_tree.hpp
	  cd succinct && cmake . && $(MAKE)