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

# take average of all runs every 100? points

mpl.style.use('seaborn')
fig, ax = plt.subplots()
#ax.errorbar(x_prog, y_prog, yerr=10)

for i, zero in enumerate(zero_loc):
    if i+1 < len(zero_loc):
        ax.errorbar(x_prog[zero:zero_loc[i+1]], y_prog[zero:zero_loc[i+1]], yerr=10, errorevery=100)
    else:
        ax.errorbar(x_prog[zero:], y_prog[zero:], yerr=10, errorevery=100)

plt.show()
