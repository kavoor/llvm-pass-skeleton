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

# Create traces
fig = go.Figure()
fig.add_trace(go.Scatter(x=x1, y=y1,
                    mode='lines+markers',
                    name='vector',text="vector",
                    error_y=dict(
            type='data', # value of error bar given in data coordinates
            array=std1,
            visible=True)))
fig.add_trace(go.Scatter(x=x0, y=y0,
                    mode='lines+markers',
                    name='serial',text="serial",
                     error_y=dict(
            type='data', # value of error bar given in data coordinates
            array=std0,
            visible=True)))

fig.show()
