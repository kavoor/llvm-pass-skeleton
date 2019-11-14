import os, subprocess, time, argparse, re
cmds = ['make many_mul FUNC=many_mul SRC1=200 COUNT=10000']

results = []
for j in range(len(cmds)):
    result = []
    for num_runs in range(5):
        sum = 0.0
        for i in range(5):
            cmd = cmds[j]
            start = time.time()
            os.system(cmd)
            end = time.time()
            sum = sum + (end - start)
            
        # average
        avg = sum / (5)
        result.append(avg)
    results.append(result)

