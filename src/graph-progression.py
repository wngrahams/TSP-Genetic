import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import re
import sys

# read in filename for path progression
filename = sys.argv[1]

GRAPH_TITLE = "Traveling Salesperson Problem - "
OUTFILE = "tsp"
FILETYPE = ".pdf"
LABEL_LOC = ""

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
avg_y = []
avg_x = []
for i, zero in enumerate(zero_loc):

    if i == 0:  # first time through, just put in what we have
        # aka copy x_prog and y_prog from zero_loc[0] to zero_loc[1]
        for j in range(0, zero_loc[i+1]):
            avg_y.append(y_prog[j])
            avg_x.append((x_prog[j]))

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
            while (avg_x[current_idx] < x_prog[x_prog_idx]) and (current_idx < len(avg_x)):
                # along the way, we need to add the previous y-value to
                # indicies we pass
                avg_y[current_idx] += y_prog[x_prog_idx-1]
                current_idx += 1

            # if we reached the end of the avg_x list, just append
            if current_idx == len(avg_x):
                avg_x.append(x_prog[x_prog_idx])
                # for the new y value, add the new y-value associated with the
                # x_prog and the value in the previous avg_y bucket, then
                # subtract the previous y_prog to prevent double adding
                new_y = y_prog[x_prog_idx] \
                        + avg_y[current_idx-1] \
                        - y_prog[x_prog_idx-1]
                assert isinstance(new_y, float)
                avg_y.append(new_y)

            # if the x_prog already has a corressponding index in the avg_x
            # list, just add the corressponding y_prog
            if avg_x[current_idx] == x_prog[x_prog_idx]:
                avg_y[current_idx] += y_prog[x_prog_idx]

            # if it doesn't yet have an index, (aka if we pass the spot
            # where it should be), add it into the list
            if avg_x[current_idx] > x_prog[x_prog_idx]:
                avg_x.insert(current_idx, x_prog[x_prog_idx])
                # for the new y value, add the new y-value associated with the
                # x_prog and the value in the previous avg_y bucket, then
                # subtract the previous y_prog to prevent double adding
                new_y = y_prog[x_prog_idx] \
                        + avg_y[current_idx-1] \
                        - y_prog[x_prog_idx-1]
                assert isinstance(new_y, float)
                avg_y.insert(current_idx, new_y)

            current_idx += 1

for i, tot in enumerate(avg_y):
    avg_y[i] = tot/len(zero_loc)

print("making graph")

# sloppy errorbars for now :(
# yerr = avg_x.copy()
#
# y_zero = []
# for zero in zero_loc:
#     y_zero.append(y_prog[zero])
#
# yerr[0:20000] = [np.std(y_zero)/math.sqrt(len(zero_loc))] * 20000
#
# y_one = []
#for

mpl.style.use('seaborn')
fig, ax = plt.subplots()
ax.errorbar(avg_x, avg_y, yerr=20, errorevery=20000, capsize=3.5,
            capthick=0.75, linewidth=0.75, label='Random Search')
plt.legend(loc=LABEL_LOC)
plt.title(GRAPH_TITLE)
plt.xlabel("Number of Evaluations")
plt.ylabel("Path Distance")

plt.savefig(OUTFILE)

plt.show()
