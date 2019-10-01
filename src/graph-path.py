import matplotlib.pyplot as plt
import re
import sys

SHORTEST = 0
LONGEST = 1


# returns a list
def index_file(file):
    # read file once into memory so we can access specific lines later
    # citation: stackoverflow.com/questions/620367/how-to-jump-to-a-particular-line-in-a-huge-text-file
    file.seek(0)
    offset = 0
    # line_num = 0
    line_idx = []
    for line in file:
        line_idx.append(offset)
        offset += len(line)
    # num_trials = int(line_num / (num_points+1))
    fi.seek(0)
    # end of citation

    return line_idx


def lt_gt(lhs, rhs, mode=str):
    return (lambda: (lhs < rhs), lambda: (lhs > rhs))[mode == SHORTEST]()

# read from file to get initial points
file_initial = sys.argv[2]
amt_to_graph = len(sys.argv) - 3

num_points = 0
num_trials = 0
mode = 0

for i, arg in enumerate(sys.argv):
    if i == 0:  # do nothing for program name arg
        continue
    elif i == 1:
        # shortest or longest path
        if sys.argv[1] == 'shortest':
            mode = SHORTEST
        elif sys.argv[1] == 'longest':
            mode = LONGEST
        else:
            print("invalid mode")
            exit(1)
    elif i == 2:
        # get number of points in file
        with open(file_initial) as f1:
            j = -1
            for j, line in enumerate(f1):
                pass
            num_points = j + 1
    else:
        with open(sys.argv[i]) as fi:

            filename_split = re.split("-", sys.argv[i])
            search_type = filename_split[1]

            line_offset = index_file(fi)
            num_trials = int(len(line_offset) / (num_points+1))
            fi.seek(0)

            # compare total path lengths, find the shortest
            optimal_len = float('inf')
            optimal_trial = -1
            for j in range(0, num_trials):
                fi.seek(line_offset[j*(num_points+1)])
                length = float(fi.readline())
                if lt_gt(length, optimal_len):
                    optimal_len = length
                    optimal_trial = j

            idx = []
            for j in range(optimal_trial*(num_points+1)+1, optimal_trial*(num_points+1) + num_points + 1):
                fi.seek(line_offset[j])
                idx.append(int(fi.readline()))

            x_arg = []
            y_arg = []
            with open(file_initial) as points_file:
                initial_line_offset = index_file(points_file)
                # points_file.seek(initial_line_offset[1])
                # print(points_file.readline())

                for j in idx:
                    # print("j: " + str(j))
                    # print(initial_line_offset[j])
                    points_file.seek(initial_line_offset[j])
                    line_to_split = points_file.readline()
                    # print(line_to_split)
                    line_split = re.split("[\t \n]", line_to_split)
                    # print(line_split)
                    x_arg.append(float(line_split[0]))
                    y_arg.append(float(line_split[1]))

            # return to the beginning:
            x_arg.append(x_arg[0])
            y_arg.append(y_arg[0])

            plt.scatter(x_arg, y_arg, s=5)
            plt.plot(x_arg, y_arg, linewidth=0.75)
            firstword = ""
            if mode == SHORTEST:
                firstword = 'Shortest'
            else:
                firstword = 'Longest'
            plt.title(firstword + " Path found using " + search_type + " search.\n Length = " + str(optimal_len))
            plt.show()



