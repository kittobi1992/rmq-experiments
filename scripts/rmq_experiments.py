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
reference="RMQ_SDSL_SCT"

length=8
seq_type="random"
delta = 0
count_cache_misses=0


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
    cmd = ['./executer/rmq_experiment.o',sequence,str(len(query))]
    cmd.extend(query)
    cmd += [str(count_cache_misses)]
    res = exe(cmd)
    return [grep(res,'QUERY_RESULT').split('\n'),grep(res,'CONSTRUCTION_RESULT').split('\n')]


def get_query_stats(out):
    res = [str(out.split('Algo=')[1].split()[0])]
    res += [int(out.split('N=')[1].split()[0])]
    res += [float(out.split('Range=')[1].split()[0])]
    res += [float(out.split('Time=')[1].split()[0])]
    res += [int(out.split('Misses=')[1].split()[0])]
    return res

def get_construction_stats(out):
    res = [str(out.split('Algo=')[1].split()[0])]
    res += [int(out.split('N=')[1].split()[0])]
    res += [float(out.split('ConstructTime=')[1].split()[0])]
    res += [float(out.split('BitsPerElement=')[1].split()[0])]
    return res

def check_results():
    files = glob.glob("benchmark/*.txt")
    reference_file = "benchmark/"+reference+".txt"
    for i in range(0,len(files)):
        diff_cmd = ['diff',files[i],reference_file]
        check_res = exe(diff_cmd);
        if len(check_res) > 0:
            print("Validate Correctness: FAIL")
            sys.exit()
    print("Validate Correctness: CORRECT")


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
    N = []
    for i in range(4,P+1):
        N += [pow(10,i)]
    print 'Experiment\n============'
    res = []
    query_res = []
    construct_res = []
    for n in N:
        print 'Sequence length N=10^'+str(int(np.log10(n)))+' and ...'
        seq = 'benchmark/' + str(n) + '.seq'
        
        #Create Sequence of length n
        create_sequence(n,1,n,seq);
         
        #Create Query-Files
        query_files = []
        Q = []
        for i in range(1,int(np.log10(n))):
            qry = 'benchmark/' + str(pow(10,i)) + '.qry'
            query_files += [qry]
            Q += [pow(10,i)]
            create_query(n,num_query,pow(10,i),qry);   
        print "    ... Query Ranges R="+str(Q)+"."
        
        #Create HTML-Folder for Memory-Usage
        try: os.stat("HTML/");
        except: os.mkdir("HTML/");
        
        #Execute Benchmark
        benchmark_res = execute_rmq_benchmark(seq,query_files)
        for q in benchmark_res[0]:
            query_res.append(get_query_stats(q));
        for c in benchmark_res[1]:
            construct_res.append(get_construction_stats(c));
            
        #Validate Result of Benchmark
        check_results()
        
        #Clean-Up
        delete_folder_content("benchmark/")
        shutil.move("HTML/",dirname + "HTML_10^"+(str(int(np.log10(n)))))
        print '\n'

    #Construct CSV-Table with Query and Construction results
    cols_query = ['Algo','N','Range','Time','Misses']
    df_query = pd.DataFrame(query_res,columns=cols_query)
    cols_construct = ['Algo','N','ConstructTime','BPE']
    df_construct = pd.DataFrame(construct_res,columns=cols_construct)
    
    df_query.to_csv(dirname + 'query_result.csv')
    df_construct.to_csv(dirname + 'construct_result.csv');
    delete_folder_content("benchmark/")

def setup_experiment_environment():
    dirname = "results/"+str(datetime.datetime.now().date())+"_rmq_experiment_"+seq_type+"_"+str(length)+"_"+str(delta)+"/";
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
    parser.add_argument("--count_cache_misses", type=bool)
    args = parser.parse_args()
    
    if args.length != None:
        length = args.length
    if args.seq_type != None:
        seq_type = args.seq_type
    if args.delta != None:
        delta = args.delta
    if args.count_cache_misses != None:
        count_cache_misses = args.count_cache_misses
        
    print 'Configuration\n============='
    print 'Maximum Sequence Length = ' + str(pow(10,length))
    print 'Sequence Type           = ' + seq_type
    print 'Sequence Delta          = ' + str(delta)
    print 'Count Cache Misses      = ' + str(count_cache_misses)
    print '\n'
    
    dirname = setup_experiment_environment()
    experiment(length,dirname)
    
