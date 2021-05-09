#!/usr/bin/env python3

import numpy as np
import matplotlib
import time as tm

matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

def plot():

    xmin = ymin = dx = dy = size = 0
    
    linecount = 0
    try:
        with open("nodevals.nod","r") as reader:
            for item in reader.readlines():

                lin = item.strip()            
                if lin.startswith('M'):   # metadata for matrix size and interval lengths
                    lin = lin[1:]
                    (xmin, ymin, dx, dy, size) = tuple(lin.strip().split(';'))
                    sol = np.zeros((int(size), int(size)))
                    cmp = np.zeros((int(size), int(size)))
                    continue
                if (lin.endswith(';')):
                    lin = lin[:-1]

                lst = lin.split(';')
                sol[linecount] = lst
                linecount += 1

            x = y = np.arange(0.0, 1.0, 1 / len(sol))
            X, Y = np.meshgrid(x, y)
            fig = plt.figure()
            
            for i in range(int(size)):
                y = i*float(dy)
                for j in range(int(size)):
                    x = j*float(dx)
                    cmp[i][j] = sol[i][j] - (2*x*x - y*y)

            ax = plt.axes(projection='3d')
            ax.set_xlabel('X axis')
            ax.set_ylabel('Y axis')
            ax.plot_surface(X, Y, sol, cmap='viridis', edgecolor='none')

            plt.savefig('plot.jpg', dpi=300, bbox_inches='tight')
            plt.show()

    except:
        print("Requested file doesn't exist \n")
        # os.system("ls -l")


if __name__ == '__main__':

    plot()
