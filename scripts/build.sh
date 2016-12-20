cd ../../sdsl-lite/build
bash -x build.sh
cd ../../rmq
make
cd experiments
g++ -std=c++11 generators/gen_sequence.cpp -o generators/gen_sequence.o
g++ -std=c++11 generators/gen_query.cpp -o generators/gen_query.o
cd executer
#g++ -std=c++11 rmq_ferrada.cpp -o rmq_ferrada.o -O3 ../../rmqrmmBP.a
#g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib rmq_sdsl_sct.cpp -o rmq_sdsl_sct.o -lsdsl -ldivsufsort -ldivsufsort64
#g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib rmq_sdsl_bp.cpp -o rmq_sdsl_bp.o -lsdsl -ldivsufsort -ldivsufsort64
#g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib rmq_sdsl_bp_fast_1024.cpp -o rmq_sdsl_bp_fast_1024.o -lsdsl -ldivsufsort -ldivsufsort64
#g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib rmq_sdsl_bp_fast_4096.cpp -o rmq_sdsl_bp_fast_4096.o -lsdsl -ldivsufsort -ldivsufsort64
g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib rmq_experiment.cpp -o rmq_experiment.o -lsdsl -ldivsufsort -ldivsufsort64 ../../rmqrmmBP.a