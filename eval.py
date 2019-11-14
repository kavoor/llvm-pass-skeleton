import os, subprocess, time, argparse, re
import numpy as np
import matplotlib.pyplot as plt

filenames = ['Bubblesort', 'FloatMM', 'IntMM', 'Oscar', 'Perm', 'Puzzle', 'Queens', 'Quicksort', 'RealMM', 'Towers', 'Treesort']
NUM_ITERATIONS = 1

results = {}
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
    results[filename] = sum/NUM_ITERATIONS

# num_bars = len(filenames)
# mean = [5 for f in filenames]
# std = [1 for f in filenames]

# x_pos = np.arange(num_bars)
# CTEs = mean
# error = std

# # Build the plot
# fig, ax = plt.subplots()
# ax.bar(x_pos, CTEs, yerr=error, align='center', alpha=0.5, ecolor='black', capsize=10)
# ax.set_ylabel('Execution Time1')
# ax.set_xticks(x_pos)
# ax.set_xticklabels(filenames)
# ax.set_title('Execution Time2')
# ax.yaxis.grid(True)

# # Save the figure and show
# plt.tight_layout()
# plt.savefig('bar_plot_with_error_bars.png')
# plt.show()

import plotly.graph_objects as go

fig = go.Figure(data=[
    go.Bar(name='instrumented', x=filenames, y=[results[filename] for filename in filenames]),
    go.Bar(name='nah', x=filenames, y=[results[filename] for filename in filenames])
])
# Change the bar mode
fig.update_layout(barmode='group')
fig.show()