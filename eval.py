import os, subprocess, time, argparse, re
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
from tqdm import trange

# filenames = ['Bubblesort', 'FloatMM', 'IntMM', 'Oscar', 'Perm', 'Puzzle', 'Queens', 'Quicksort', 'RealMM', 'Towers', 'Treesort']
filenames = [
    "fasta" ,

    "fannkuch" ,

    "n-body" ,

    "nsieve-bits" ,

    "partialsums" ,

    "puzzle" ,

    "recursive" ,

    "spectral-norm" ,

    "almabench" ,

    "huffbench" ,

    "lpbench" ,

    "dry" ,

    "fldry" ,

    "linpack-pc" ,

    "chomp" ,


    "misr" ,

    "queens" ,

    "ReedSolomon" ,

    "dt" ,

    "evalloop" ,

    "fbench" ,

    "ffbench" ,

    "flops-1" ,

    "flops-2" ,

    "flops-3" ,

    "flops-4" ,

    "flops-5" ,

    "flops-6" ,

    "flops-7" ,

    "flops-8" ,

    "flops" ,

    "fp-convert" ,

    "himenobmtxpa" ,

    "lowercase" ,

    "mandel-2" ,

    "mandel" ,

    "matmul_f64_4x4" ,

    "oourafft" ,

    "perlin" ,

    "pi" ,

    "revertBits" ,

    "richards_benchmark" ,

    "salsa20" ,

    "whetstone" ,

    "covariance" ,

    "2mm" ,

    "cholesky" ,

    "gemver" ,

    "gesummv" ,

    "syrk" ,

    "gramschmidt" ,

    "lu" ,

    "ludcmp" ,

    "floyd-warshall" ,

    "polybench" ,

    "ackermann" ,

    "ary3" ,

    "fib2" ,

    "hash" ,

    "heapsort" ,

    "hello" ,

    "lists" ,

    "matrix" ,

    "methcall" ,

    "nestedloop" ,

    "objinst" ,

    "random" ,

    "sieve" ,

    "strcat" ,

    "Bubblesort" ,

    "FloatMM" ,

    "IntMM" ,

    "Oscar" ,

    "Perm" ,

    "Puzzle" ,

    "Queens" ,

    "Quicksort" ,

    "RealMM" ,

    "Towers" ,

    "Treesort" ,

    "2mm" ,

    "Bubblesort" ,

    "FloatMM" ,

    "IntMM" ,

    "Oscar" ,

    "Perm" ,

    "Quicksort" ,

    "RealMM" ,

    "ReedSolomon" ,

    "Towers" ,

    "Treesort" ,

    "ackermann" ,

    "almabench" ,

    "ary3" ,

    "cholesky" ,

    "chomp" ,

    "covariance" ,

    "dry" ,

    "dt" ,

    "evalloop" ,


    "fannkuch" ,

    "fasta" ,

    "fbench" ,

    "ffbench" ,

    "fib2" ,

    "fldry" ,

    "flops-1" ,

    "flops-2" ,

    "flops-3" ,

    "flops-4" ,

    "flops-5" ,

    "flops-6" ,

    "flops-7" ,

    "flops-8" ,

    "flops" ,

    "floyd-warshall" ,

    "fp-convert" ,

    "gemver" ,

    "gesummv" ,

    "gramschmidt" ,

    "hash" ,

    "heapsort" ,

    "hello" ,

    "himenobmtxpa" ,

    "huffbench" ,

    "linpack-pc" ,

    "lists" ,

    "lowercase" ,

    "lpbench" ,

    "lu" ,

    "ludcmp" ,

    "mandel-2" ,

    "mandel" ,

    "matmul_f64_4x4" ,

    "matrix" ,

    "methcall" ,

    "misr" ,

    "n-body" ,

    "nestedloop" ,

    "nsieve-bits" ,

    "objinst" ,

    "oourafft" ,

    "partialsums" ,

    "perlin" ,

    "pi" ,

    "polybench" ,

    "puzzle" ,

    "queens" ,

    "random" ,

    "recursive" ,

    "revertBits" ,

    "richards_benchmark" ,

    "salsa20" ,

    "sieve" ,

    "spectral-norm" ,

    "strcat" ,

    "syrk" ,

    "whetstone" ,

]
filenames = list(set(filenames))
NUM_ITERATIONS = 3

print("COMPILING TO LLVM")
for filename in tqdm(filenames):
    os.system('/usr/local/opt/llvm@9/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone --include-directory=test-suite/target/ test-suite/target/' + filename + '.c ')
    worked = os.path.exists('./'+filename+'.ll')
    if(not worked):
        filenames.remove(filename)

print("RUNNING -O3")
results0 = {}
for filename in tqdm(filenames):
    sum = []
    os.system('opt -O3 ' + filename + '.ll -o ' + filename + '0.ll &> /dev/null')
    os.system('rm ' + filename + 'link.ll ; rm a.out &> /dev/null')
    os.system('llvm-link rtlib.ll ' + filename + '0.ll ' + '-o ' + filename + 'link.ll')
    worked = os.path.exists(filename+'link.ll')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename)
        continue
    os.system('llc '+filename+'link.ll; clang '+filename+'link.s -o '+filename+'a.out')
    worked = os.path.exists(filename+'a.out')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename)
        continue

    os.system('opt -load build/skeleton/libSkeletonPass.* -skeleton -S ' + filename + '0.ll -o ' + filename + '1.ll &> /dev/null')
    os.system('rm '+filename+'link1.ll; rm '+filename+'a1.out &> /dev/null')
    os.system('llvm-link rtlib.ll ' + filename + '1.ll ' + '-o '+filename+'link1.ll')
    worked = os.path.exists(filename+'link1.ll')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename)
        continue
    os.system('llc '+filename+'link1.ll; clang '+filename+'link1.s -o '+filename+'a1.out')
    worked = os.path.exists(filenam+'a1.out')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename)
        continue
    
    os.system('opt -O3 ' + filename + '1.ll -o ' + filename + '2.ll > /dev/null')
    os.system('rm '+filename+'link2.ll; rm '+filename+'a2.out &> /dev/null')
    os.system('llvm-link rtlib.ll ' + filename + '2.ll ' + '-o '+filename+'link2.ll')
    worked = os.path.exists(filename+'link2.out')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename) 
        continue
    os.system('llc '+filename+'link2.ll; clang '+filename+'link2.s -o '+filename+'a2.out')
    worked = os.path.exists(filename+'a2.out')
    if(not worked):
        if filename in filenames:
            filenames.remove(filename)
        continue

    for _ in range(NUM_ITERATIONS):
        start = time.time()
        os.system(filename+'./a.out > /dev/null')
        end = time.time()
        sum.append(end-start)
    results0[filename] = sum

print("RUNNING -O3 SKELETON")
results1 = {}
for filename in tqdm(filenames):
    sum = []
    for _ in range(NUM_ITERATIONS):
        
        start = time.time()
        os.system(filename+'./a1.out > /dev/null')
        end = time.time()
        sum.append(end-start)
    results1[filename] = sum

print("RUNNING -O3 SKELETON -O3")
results2 = {}
for filename in tqdm(filenames):
    sum = []
    for _ in range(NUM_ITERATIONS):
        start = time.time()
        os.system(filename+'./a2.out > /dev/null')
        end = time.time()
        sum.append(end-start)
    results2[filename] = sum

print(results0)
print(results1)
print(results2)

for filename in filenames:
    r0 = np.mean(results0[filename])
    results0[filename] = results0[filename] / r0 
    results1[filename] = results1[filename] / r0 
    results2[filename] = results2[filename] / r0 

# print(results0)
# print(results1)
# print(results2)


import plotly.graph_objects as go

fig = go.Figure(data=[
    go.Bar(name='-O3', x=filenames, y=[np.mean(results0[filename]) for filename in filenames], error_y = dict(type = 'data', array = [np.std(results0[filename]) for filename in filenames], visible = True )),
    go.Bar(name='Profiled', x=filenames, y=[np.mean(results1[filename]) for filename in filenames], error_y = dict(type = 'data', array = [np.std(results1[filename]) for filename in filenames], visible = True )),
    go.Bar(name='Profiled and -O3', x=filenames, y=[np.mean(results2[filename]) for filename in filenames], error_y = dict(type = 'data', array = [np.std(results2[filename]) for filename in filenames], visible = True ))
])
# Change the bar mode
fig.update_layout(barmode='group')
fig.show()