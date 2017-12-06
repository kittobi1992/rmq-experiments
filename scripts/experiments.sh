min_N=4
max_N=8

############## Comparison of RMQ implementations (Time) ##############

taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=random --delta=0 --count_cache_misses=0

taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=increasing --delta=0 --count_cache_misses=0
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=increasing --delta=100 --count_cache_misses=0
##Ferrada RMQ failed for sequence length 10^5 in this case, therefore we set min length to 10^6
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=6 --max_length=$max_N --seq_type=increasing --delta=10000 --count_cache_misses=0

taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=decreasing --delta=0 --count_cache_misses=0
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=decreasing --delta=100 --count_cache_misses=0
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=decreasing --delta=10000 --count_cache_misses=0


############### Comparison of RMQ implementations (Cache Misses) ##############
#
taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=random --delta=0 --count_cache_misses=1
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=increasing --delta=0 --count_cache_misses=1
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=0 --min_length=$min_N --max_length=$max_N --seq_type=decreasing --delta=0 --count_cache_misses=1
#
#
############### Comparison of SDSL implementations (Time) ##############
#
taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=1 --min_length=$min_N --max_length=$max_N --seq_type=random --delta=0 --count_cache_misses=0
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=1 --min_length=$min_N --max_length=$max_N --seq_type=increasing --delta=0 --count_cache_misses=0
#taskset 0x1 python scripts/rmq_experiments.py --compare_sdsl=1 --min_length=$min_N --max_length=$max_N --seq_type=decreasing --delta=0 --count_cache_misses=0
#
#
############### Application: Suffix Tree Traversal ##############
#
taskset 0x1 python scripts/lcp_experiments.py
#
############### Application: Distinct Color ##############
#
#taskset 0x1 python scripts/distinct_color_experiments.py