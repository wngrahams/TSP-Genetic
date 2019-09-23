import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import re
import sys

# read in filename for path progression
filename = sys.argv[1]

GRAPH_TITLE = "Traveling Salesperson Problem - "
OUTFILE = "./output/tsp"
FILETYPE = ".pdf"
LABEL_LOC = ""
CONFIDENCE_INTERVAL = 2.567  # 99%

if sys.argv[2] == 'longest':
    GRAPH_TITLE += "Longest Path"
    OUTFILE += "-longest" + FILETYPE
    LABEL_LOC = "lower right"
else:
    GRAPH_TITLE += "Shortest Path"
    OUTFILE += "-shortest" + FILETYPE
    LABEL_LOC = "upper right"

GRAPH_TITLE += "\nAverage performance over 40 trials"

x_prog = []
y_prog = []
counter = 0
zero_loc = []
with open(filename, 'r') as fp:
    for line in fp:
        line_split = re.split("[\t \n]", line)
        x_prog.append(int(line_split[0]))
        y_prog.append(float(line_split[1]))

        if int(line_split[0]) == 0:
            zero_loc.append(counter)

        counter += 1

# take average of all runs
avg_x = []
bucket_list = []
for i, zero in enumerate(zero_loc):

    if i == 0:  # first time through, just put in what we have
        # aka copy x_prog and y_prog from zero_loc[0] to zero_loc[1]
        for j in range(0, zero_loc[i+1]):
            avg_x.append((x_prog[j]))
            bucket_list.append([y_prog[j]])

    else:
        current_idx = 0  # current index of avg_x
        x_range = range(0)
        if i+1 < len(zero_loc):
            x_range = range(zero, zero_loc[i+1])
        else:
            x_range = range(zero, len(x_prog))

        for x_prog_idx in x_range:
            # get to the place in avg_x where we would expect to put the
            # x_prog
            while (current_idx < len(avg_x)) and (avg_x[current_idx] < x_prog[x_prog_idx]):
                # along the way, we need to add the previous y-value to
                # indicies we pass
                (bucket_list[current_idx]).append(y_prog[x_prog_idx-1])
                current_idx += 1

            # if we reached the end of the avg_x list, just append
            if current_idx == len(avg_x):
                avg_x.append(x_prog[x_prog_idx])
                # for the new y value, add the new y-value associated with the
                # x_prog and the value in the previous avg_y bucket, then
                # subtract the previous y_prog to prevent double adding
                bucket_list.append([y_prog[x_prog_idx]])
                for old_item in bucket_list[current_idx-1]:
                    if old_item != y_prog[x_prog_idx-1]:
                        (bucket_list[current_idx]).append(old_item)

            # if the x_prog already has a corresponding index in the avg_x
            # list, just add the corresponding y_prog
            if avg_x[current_idx] == x_prog[x_prog_idx]:
                (bucket_list[current_idx]).append(y_prog[x_prog_idx])

            # if it doesn't yet have an index, (aka if we pass the spot
            # where it should be), add it into the list)
            if avg_x[current_idx] > x_prog[x_prog_idx]:
                avg_x.insert(current_idx, x_prog[x_prog_idx])
                # for the new y value, add the new y-value associated with the
                # x_prog and the value in the previous avg_y bucket, then
                # subtract the previous y_prog to prevent double adding
                bucket_list.insert(current_idx, [y_prog[x_prog_idx]])
                for old_item in bucket_list[current_idx-1]:
                    if old_item != y_prog[x_prog_idx-1]:
                        (bucket_list[current_idx]).append(old_item)

            current_idx += 1

avg_y = []
err_y = []

for bucket in bucket_list:
    tot = 0
    n = len(bucket)
    for y in bucket:
        tot += y
    avg_y.append(tot/n)
    err_y.append((np.std(bucket)/np.sqrt(n)) * CONFIDENCE_INTERVAL)

print("making graph")

mpl.style.use('seaborn')
fig, ax = plt.subplots()
ax.errorbar(avg_x, avg_y, yerr=err_y, errorevery=10000, capsize=3.5,
            capthick=0.75, linewidth=0.75, label='Random Search')
plt.legend(loc=LABEL_LOC)
plt.title(GRAPH_TITLE)
plt.xlabel("Number of Evaluations")
plt.ylabel("Path Distance")

plt.savefig(OUTFILE)

plt.show()
