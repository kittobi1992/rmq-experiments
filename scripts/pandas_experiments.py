from subprocess import check_output
from prettytable import PrettyTable
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import re, sys
import os, glob
import shutil

programs = ['./executer/rmq_ferrada.o','./executer/rmq_sdsl_sct.o','./executer/rmq_sdsl_bp.o','./executer/rmq_sdsl_bp_fast_1024.o','./executer/rmq_sdsl_bp_fast_4096.o']
algos = ['RMQ_FERRADA', 'RMQ_SDSL_SCT','RMQ_SDSL_BP','RMQ_SDSL_BP_FAST_1024','RMQ_SDSL_BP_FAST_4096']
num_query=1000

def exe(cmd):
    try:
        return check_output(cmd)
    except Exception, e:
        print 'Error while running `%s`: %s' % (' '.join(cmd), e)
        raise
    
def create_sequence(n,a,b,p,d,f):
    return exe(['./generators/gen_sequence.o',
         '-n', str(n),
         '-a', str(a),
         '-b', str(b),
         '-p', str(p),
         '-d', str(d),
         '-f', f]);
    
def create_query(n,q,r,f):
    return exe(['./generators/gen_query.o',
         '-n', str(n),
         '-q', str(q),
         '-r', str(r),
         '-f', f]);

def grep(s,pattern):
    return '\n'.join(re.findall(r'^.*%s.*?$'%pattern,s,flags=re.M))

def execute_rmq_benchmark(program, sequence, query):
    return grep(exe([program,sequence,query]),'RESULT')


def get_stats(N,out):
    res = [str(out.split('Algo=')[1].split()[0])]
    res += N
    res += [float(out.split('ConstructTime=')[1].split()[0])]
    res += [float(out.split('BitsPerElement=')[1].split()[0])]
    res += [float(out.split('AvgQueryTime=')[1].split()[0])]
    res += [float(out.split('MedianQueryTime=')[1].split()[0])]
    res += [float(out.split('MinQueryTime=')[1].split()[0])]
    res += [float(out.split('MaxQueryTime=')[1].split()[0])]
    res += [float(out.split('StandardDerivation=')[1].split()[0])]
    return res

def delete_benchmarks():
    files = glob.glob('benchmark/*')
    for f in files:
        os.remove(f)

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

def experiment1():
    N=[10000,100000,1000000,10000000]
    Q=10000
    print 'Experiment 1\n============'
    des = 'Sequence length N='+str(N)+' and Query Range is 10000'
    print des
    res = []
    for n in N:
        seq = 'benchmark/' + str(n) + '.seq'
        qry = 'benchmark/' + str(n) + '.qry'
        create_sequence(n,1,n,0,1,seq);
        create_query(n,num_query,Q,qry);
        for p in programs:
            out = execute_rmq_benchmark(p,seq,qry);
            stats = get_stats([n,Q,0],out)
            res += [stats]
    cols = ['Algo','N','Range','Delta','ConstructTime','BPE','Avg','Median', 'Min', 'Max', 'Sigma']
    df = pd.DataFrame(res,columns=cols)
    plot_data_frame(df,'N','Avg', "x: Sequence length (random values) - y: Avg Query Time in microseconds",'results/experiment1/avg.png', N)
    plot_data_frame(df,'N','Median', "x: Sequence length (random values) - y: Median Query Time in microseconds",'results/experiment1/median.png', N)
    plot_data_frame(df,'N','Min', "x: Sequence length (random values) - y: Min Query Time in microseconds",'results/experiment1/min.png', N)
    plot_data_frame(df,'N','Max', "x: Sequence length (random values) - y: Max Query Time in microseconds",'results/experiment1/max.png', N)  
    plot_data_frame(df,'N','Sigma', "x: Sequence length (random values) - y: Standard Derivation in microseconds",'results/experiment1/sigma.png', N)  
    plot_data_frame(df,'N','BPE', "x: Sequence length (random values) - y: Bits per Element",'results/experiment1/bpe.png', N)     
    plot_data_frame(df,'N','ConstructTime', "x: Sequence length (random values) - y: Construction time in milliseconds",'results/experiment1/construct.png', N)  
    df.to_csv('table.csv');
    print df
    print '\n'
    delete_benchmarks()
    
def experiment2():
    N=10000000
    seq = 'benchmark/' + str(N) + '.seq'
    create_sequence(N,1,N,0,1,seq);
    Q=[10,100,1000,10000,100000,1000000]
    print 'Experiment 2\n============'
    print 'Sequence length N='+str(N)+" and Query Range Q = " +str(Q)
    res = []
    for q in Q:
        qry = 'benchmark/' + str(q) + '.qry'
        create_query(N,num_query,q,qry);
        for p in programs:
            out = execute_rmq_benchmark(p,seq,qry);
            stats = get_stats([N,q,0],out)
            res += [stats]           
    cols = ['Algo','N','Range','Delta','ConstructTime','BPE','Avg','Median', 'Min', 'Max', 'Sigma']
    df = pd.DataFrame(res,columns=cols)
    plot_data_frame(df,'Range','Avg', "N=10^7, x: Query Interval Range  - y: Avg Query Time in microseconds",'results/experiment2/avg.png',Q)
    plot_data_frame(df,'Range','Median', "N=10^7, x: Query Interval Range  - y: Median Query Time in microseconds",'results/experiment2/median.png',Q)
    plot_data_frame(df,'Range','Min', "N=10^7, x: Query Interval Range  - y: Min Query Time in microseconds",'results/experiment2/min.png',Q)
    plot_data_frame(df,'Range','Max', "N=10^7, x: Query Interval Range - y: Max Query Time in microseconds",'results/experiment2/max.png',Q)  
    plot_data_frame(df,'Range','Sigma', "N=10^7, x: Query Interval Range - y: Standard Derivation in microseconds",'results/experiment2/sigma.png',Q)  
    print df
    print '\n'
    delete_benchmarks()
    
def experiment3():
    N=1000000
    Q=10000
    D=[1,10,100,1000,10000,100000]
    print 'Experiment 3\n============'
    print 'Pseudo-sorted increasing sequence length N='+str(N)+" and Query Range Q = " +str(Q)+" and Delta D = " + str(D)
    qry = 'benchmark/' + str(Q) + '.qry'
    create_query(N,num_query,Q,qry);
    res = []
    for d in D:
        seq = 'benchmark/' + str(d) + '.seq'
        create_sequence(N,0,1,1,d,seq);
        for p in programs:
            out = execute_rmq_benchmark(p,seq,qry);
            stats = get_stats([N,Q,d],out)
            res += [stats]          
    cols = ['Algo','N','Range','Delta','ConstructTime','BPE','Avg','Median', 'Min', 'Max', 'Sigma']
    df = pd.DataFrame(res,columns=cols)
    plot_data_frame(df,'Delta','Avg', "N=10^6, x: Sequence length (pseudo-sorted increasing values) - y: Avg Query Time in microseconds",'results/experiment3/avg.png',D)
    plot_data_frame(df,'Delta','Median', "N=10^6, x: Sequence length (pseudo-sorted increasing values)  - y: Median Query Time in microseconds",'results/experiment3/median.png',D)
    plot_data_frame(df,'Delta','Min', "N=10^6, x: Sequence length (pseudo-sorted increasing values)  - y: Min Query Time in microseconds",'results/experiment3/min.png',D)
    plot_data_frame(df,'Delta','Max', "N=10^6, x: Sequence length (pseudo-sorted increasing values) - y: Max Query Time in microseconds",'results/experiment3/max.png',D)  
    plot_data_frame(df,'Delta','Sigma', "N=10^6, x: Sequence length (pseudo-sorted increasing values)  - y: Standard Derivation in microseconds",'results/experiment3/sigma.png',D)  
    plot_data_frame(df,'Delta','BPE', "N=10^6, x: Sequence length (pseudo-sorted increasing values)  - y: Bits per Element",'results/experiment3/bpe.png',D)     
    plot_data_frame(df,'Delta','ConstructTime', "N=10^6, x: Sequence length (pseudo-sorted increasing values)  - y: Construction time in milliseconds",'results/experiment3/construct.png',D)   
    print df
    print '\n'
    delete_benchmarks()
    
def experiment4():
    N=1000000
    Q=10000
    D=[1,10,100,1000,10000,100000]
    print 'Experiment 4\n============'
    print 'Pseudo-sorted decreasing sequence length N='+str(N)+" and Query Range Q = " +str(Q)+" and Delta D = " + str(D)
    qry = 'benchmark/' + str(Q) + '.qry'
    create_query(N,num_query,Q,qry);
    res = []
    for d in D:
        seq = 'benchmark/' + str(d) + '.seq'
        create_sequence(N,0,1,2,d,seq);
        for p in programs:
            out = execute_rmq_benchmark(p,seq,qry);
            stats = get_stats([N,Q,d],out)
            res += [stats]           
    cols = ['Algo','N','Range','Delta','ConstructTime','BPE','Avg','Median', 'Min', 'Max', 'Sigma']
    df = pd.DataFrame(res,columns=cols)
    plot_data_frame(df,'Delta','Avg', "N=10^6, x: Sequence length (pseudo-sorted decreasing values) - y: Avg Query Time in microseconds",'results/experiment4/avg.png',D)
    plot_data_frame(df,'Delta','Median', "N=10^6, x: Sequence length (pseudo-sorted decreasing values)  - y: Median Query Time in microseconds",'results/experiment4/median.png',D)
    plot_data_frame(df,'Delta','Min', "N=10^6, x: Sequence length (pseudo-sorted decreasing values)  - y: Min Query Time in microseconds",'results/experiment4/min.png',D)
    plot_data_frame(df,'Delta','Max', "N=10^6, x: Sequence length (pseudo-sorted decreasing values) - y: Max Query Time in microseconds",'results/experiment4/max.png',D)  
    plot_data_frame(df,'Delta','Sigma', "N=10^6, x: Sequence length (pseudo-sorted decreasing values)  - y: Standard Derivation in microseconds",'results/experiment4/sigma.png',D)  
    plot_data_frame(df,'Delta','BPE', "N=10^6, x: Sequence length (pseudo-sorted decreasing values)  - y: Bits per Element",'results/experiment4/bpe.png',D)     
    plot_data_frame(df,'Delta','ConstructTime', "N=10^6, x: Sequence length (pseudo-sorted decreasing values)  - y: Construction time in milliseconds",'results/experiment4/construct.png',D)  
    print df
    print '\n'
    delete_benchmarks()


if __name__ == '__main__':
    experiment1()
    experiment2()
    experiment3()
    experiment4()
    shutil.make_archive("experiments", 'zip', "results")