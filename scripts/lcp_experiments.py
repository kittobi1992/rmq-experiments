from subprocess import check_output
import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse
import re, sys
import os, glob
import shutil

lcp_benchmark="pizza&chilli/"
build_dir = "pizza&chilli/build/"

def exe(cmd):
    try:
        return check_output(cmd)
    except Exception, e:
        print 'Error while running `%s`: %s' % (' '.join(cmd), e)
        raise
    

def grep(s,pattern):
    return '\n'.join(re.findall(r'^.*%s.*?$'%pattern,s,flags=re.M))

def execute_lcp_benchmark(benchmark):
    cmd = ['./executer/lcp_experiment.o',benchmark,build_dir]
    res = exe(cmd)
    return grep(res,'LCP_RESULT').split('\n')


def get_lcp_stats(out):
    res = [str(out.split('Benchmark=')[1].split()[0])]
    res += [str(out.split('Algo=')[1].split()[0])]
    res += [float(out.split('Time=')[1].split()[0])]
    res += [long(out.split('NumQueries=')[1].split()[0])]
    return res


def delete_folder_content(experiment_dir):
    for root, dirs, files in os.walk(experiment_dir, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))

def seperator(N):
    sep = []
    for i in range(0,N):
        sep += ['--------']
    return sep


def experiment(dirname):
    print 'LCP-Experiment\n============'
    lcp_res = []
    
    benchmarks = glob.glob(lcp_benchmark+"*")
    for benchmark in benchmarks:
        if(os.path.isdir(benchmark)):
            continue
        print("Execute benchmark " + benchmark + "...")
        benchmark_res = execute_lcp_benchmark(benchmark)
        for r in benchmark_res:
            lcp_res.append(get_lcp_stats(r));    
        
    cols_lcp = ["Benchmark","Algo","Time","NumQueries"]
    df_lcp = pd.DataFrame(lcp_res,columns=cols_lcp)
    df_lcp.to_csv(dirname + '/lcp_result.csv')
    

def setup_experiment_environment():
    dirname = "results/"+str(datetime.datetime.now().date())+"_lcp_experiment";
    try: os.stat(dirname);
    except: os.mkdir(dirname);
    delete_folder_content(dirname)
    return dirname


if __name__ == '__main__':

    dirname = setup_experiment_environment()
    experiment(dirname)
    
