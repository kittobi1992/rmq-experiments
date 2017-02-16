baseurl='http://algo2.iti.kit.edu/gog/projects/rmq'

files='
./2017-02-13_rmq_experiment_random_10_0/query_result.csv
./2017-02-13_rmq_experiment_random_10_0/construct_result.csv
./2017-01-05_rmq_experiment_random_9_0_timings/timing_result.csv
./2017-01-05_rmq_experiment_random_10_0/query_result.csv
./2017-01-05_rmq_experiment_random_10_0/construct_result.csv
./2017-01-06_rmq_experiment_random_10_0/query_result.csv
./2017-01-06_rmq_experiment_random_10_0/construct_result.csv
./2017-01-05_rmq_experiment_decreasing_9_0/query_result.csv
./2017-01-05_rmq_experiment_decreasing_9_0/construct_result.csv
./2017-01-06_rmq_experiment_increasing_9_10000/query_result.csv
./2017-01-06_rmq_experiment_increasing_9_10000/construct_result.csv
./2017-02-08_lcp_experiment/lcp_result.csv
./2017-01-05_rmq_experiment_increasing_9_100/query_result.csv
./2017-01-05_rmq_experiment_increasing_9_100/construct_result.csv
./2017-01-05_rmq_experiment_decreasing_9_10000/query_result.csv
./2017-01-05_rmq_experiment_decreasing_9_10000/construct_result.csv
./2017-01-05_rmq_experiment_increasing_9_0/query_result.csv
./2017-01-05_rmq_experiment_increasing_9_0/construct_result.csv
./2017-01-05_rmq_experiment_decreasing_9_100/query_result.csv
./2017-01-05_rmq_experiment_decreasing_9_100/construct_result.csv
./2017-01-05_rmq_experiment_random_10_0_timings/timing_result.csv
'

for f in ${files}; do
    echo "${f}"
    dir=$(dirname "${f}")
    name=$(basename "${f}")

    mkdir -p "${dir}"
    cd "${dir}"
        echo "curl -O ${baseurl}/${f}"
        curl -O ${baseurl}/${f}
    cd ..
done
