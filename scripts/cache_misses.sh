#Sequence length
N=100000
#Number of generated queries per range
Q=10000
#Query Ranges
declare -a R=(10 100 1000 10000)
#Delta for Pseudo-Increasing and -Decreasing sequences
D=10000
#Sequence type: (= 0) Random, (= 1) Pseudo-Increasing, (= 2) Pseudo-Decreasing
P=1


#-------------------------------------------------------------------------------------------------------------------------------

#Generating sequence
./generators/gen_sequence.o -n $N -a 0 -b $N -p $P -d $D -f benchmark/$N.seq > /dev/null

#Generating queries
for r in "${R[@]}"
do
    ./generators/gen_query.o -n $N -q $Q -r $r -f benchmark/$r.qry > /dev/null
done


query_files=""
for r in "${R[@]}"
do
    query_files+="benchmark/$r.qry " 
done

#Execute without query 
perf stat -e cache-references,cache-misses ./executer/rmq_experiment.o benchmark/$N.seq ${#R[@]} $query_files 0 > /dev/null

#Execute with query 
perf stat -e cache-references,cache-misses ./executer/rmq_experiment.o benchmark/$N.seq ${#R[@]} $query_files 1 > /dev/null

rm -rf benchmark/*