from subprocess import check_output
import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse
import re, sys
import os, glob
import shutil

num_query=10000

length=8
seq_type="random"
delta = 0


def exe(cmd):
    try:
        return check_output(cmd)
    except Exception, e:
        print 'Error while running `%s`: %s' % (' '.join(cmd), e)
        raise
    
def create_sequence(n,a,b,f):
    seq_t = 0
    if seq_type == 'increasing':
        seq_t = 1
    if seq_type == 'decreasing':
        seq_t = 2
    if seq_type == 'equal':
        seq_t = 3
    return exe(['./generators/gen_sequence.o',
         '-n', str(n),
         '-a', str(a),
         '-b', str(b),
         '-p', str(seq_t),
         '-d', str(delta),
         '-f', f]);
    
def create_query(n,q,r,f):
    return exe(['./generators/gen_query.o',
         '-n', str(n),
         '-q', str(q),
         '-r', str(r),
         '-f', f]);


def grep(s,pattern):
    return '\n'.join(re.findall(r'^.*%s.*?$'%pattern,s,flags=re.M))

def execute_rmq_benchmark(sequence, query):
    cmd = ['./executer/timing_experiment.o',sequence,str(len(query))]
    cmd.extend(query)
    res = exe(cmd)
    return grep(res,'TIMING_RESULT').split('\n')


def get_query_stats(out):
    res = [float(out.split('Range=')[1].split()[0])]
    res += [float(out.split('Rank=')[1].split()[0])]
    res += [float(out.split('Scan=')[1].split()[0])]
    res += [float(out.split('Select=')[1].split()[0])]
    res += [float(out.split('Sparse_RMQ=')[1].split()[0])]
    res += [float(out.split('min_excess=')[1].split()[0])]
    res += [float(out.split('min_excess_idx=')[1].split()[0])]
    res += [float(out.split('Other=')[1].split()[0])]
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

def plot_data_frame(df, x, y, t, f, xticks):
    dfs = [];
    for algo in algos:
        data = df[df.Algo == algo].loc[:,[x,y]]
        data.columns = [x,algo]
        dfs += [data]
    plot_df = dfs[0]
    for i in range(1,len(dfs)):
        plot_df = pd.merge(plot_df,dfs[i],on=x);
    ax = plot_df.plot(kind='line', x=x, title=t, xticks=xticks, logx=True);
    fig = ax.get_figure();
    fig.savefig(f);

def experiment(P,dirname):
    N = pow(10,P)
    print 'Experiment\n============'
    res = []
    query_res = []
    construct_res = []
    print 'Sequence length N=10^'+str(int(np.log10(N)))+' and ...'
    seq = 'benchmark/' + str(N) + '.seq'
        
    #Create Sequence of length n
    create_sequence(N,1,100,seq);
        
    #Create Query-Files
    query_files = []
    Q = []
    for i in range(1,int(np.log10(N))):
        qry = 'benchmark/' + str(pow(10,i)) + '.qry'
        query_files += [qry]
        Q += [pow(10,i)]
        create_query(N,num_query,pow(10,i),qry);   
    print "    ... Query Ranges R="+str(Q)+"."
        
    #Create HTML-Folder for Memory-Usage
    try: os.stat("HTML/");
    except: os.mkdir("HTML/");
        
    #Execute Benchmark
    benchmark_res = execute_rmq_benchmark(seq,query_files)
    for q in benchmark_res:
        query_res.append(get_query_stats(q));
        
    #Clean-Up
    delete_folder_content("benchmark/")
    shutil.move("HTML/",dirname + "HTML_10^"+(str(int(np.log10(N)))))
    print '\n'

    #Construct CSV-Table with Query and Construction results
    cols_query = ['Range','Rank','Scan','Select','Sparse_RMQ',"min_excess","min_excess_idx","Other"]
    df_query = pd.DataFrame(query_res,columns=cols_query)
    
    df_query.to_csv(dirname + 'timing_result.csv')
    delete_folder_content("benchmark/")

def setup_experiment_environment():
    dirname = "results/"+str(datetime.datetime.now().date())+"_rmq_experiment_"+seq_type+"_"+str(length)+"_"+str(delta)+"_timings/";
    try: os.stat(dirname);
    except: os.mkdir(dirname);
    delete_folder_content("benchmark/")
    delete_folder_content(dirname)
    return dirname


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--length",type=int);
    parser.add_argument("--seq_type", type=str)
    parser.add_argument("--delta", type=int)
    args = parser.parse_args()
    
    if args.length != None:
        length = args.length
    if args.seq_type != None:
        seq_type = args.seq_type
    if args.delta != None:
        delta = args.delta
        
    print 'Configuration\n============='
    print 'Sequence Length = ' + str(pow(10,length))
    print 'Sequence Type           = ' + seq_type
    print 'Sequence Delta          = ' + str(delta)
    print '\n'
    
    dirname = setup_experiment_environment()
    experiment(length,dirname)
    
