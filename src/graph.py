import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import re
import sys

point_list = []

# read in filename for data points
filename = sys.argv[1]
with open(filename, 'r') as fp:
    for line in fp:
        point_str = re.split("[\t \n]", line)
        point = float(point_str[0]), float(point_str[1])
        point_list.append(point)

# print(point_list)

# read in filename for path progression
filename = sys.argv[2]
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
print()
print(zero_loc)
print()
avg_y = []
avg_x = []
# avg_n = []
for i, zero in enumerate(zero_loc):

    if i == 0:  # first time through, just put in what we have
        # aka copy x_prog and y_prog from zero_loc[0] to zero_loc[1]
        for j in range(0, zero_loc[i+1]):
            avg_y.append(y_prog[j])
            avg_x.append((x_prog[j]))
            # avg_n.append(1)

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
            while avg_x[current_idx] < x_prog[x_prog_idx]:
                # along the way, we need to add the previous y-value to
                # indicies we pass
                avg_y[current_idx] += y_prog[x_prog_idx-1]
                current_idx += 1

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

mpl.style.use('seaborn')
fig, ax = plt.subplots()
#ax.errorbar(x_prog, y_prog, yerr=10)

for i, zero in enumerate(zero_loc):
    if i+1 < len(zero_loc):
        #ax.errorbar(x_prog[zero:zero_loc[i+1]], y_prog[zero:zero_loc[i+1]], yerr=10, errorevery=100)
        pass
    else:
        #ax.errorbar(x_prog[zero:], y_prog[zero:], yerr=10, errorevery=100)
        pass

ax.errorbar(avg_x, avg_y, yerr=10, errorevery=len(avg_x)/10, ecolor='black')

plt.show()
