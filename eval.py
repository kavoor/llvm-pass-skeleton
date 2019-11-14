import os, subprocess, time, argparse, re
cmds = ['clang -S -emit-llvm -Xclang -disable-O0-optnone',
        'opt -load build/skeleton/libSkeletonPass.* -skeleton -S ']

filenames = ['Bubblesort', 'FloatMM', 'IntMM', 'Oscar', 'Perm', 'Puzzle', 'Queens', 'Quicksort', 'RealMM', 'Towers', 'Treesort']
NUM_ITERATIONS = 5


for filename in filenames:
    sum = 0
    for _ in range(NUM_ITERATIONS):
        # filename = 'test-suite/Stanford/' + filename
        os.system('/usr/local/opt/llvm@9/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone test-suite/Stanford/' + filename + '.c ')
        os.system('opt -load build/skeleton/libSkeletonPass.* -skeleton -S ' + filename + '.ll -o ' + filename + '1.ll > /dev/null')
        os.system('llvm-link rtlib.ll ' + filename + '1.ll ' + '-o link.ll')
        os.system('llc link.ll; clang link.s')
        start = time.time()
        os.system('./a.out > /dev/null')
        end = time.time()
        print(end-start)
        sum += end-start
    print("AVERAGE FOR ",filename, ' : ', sum/NUM_ITERATIONS)
