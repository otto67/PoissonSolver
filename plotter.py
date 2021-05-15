#!/usr/bin/env python3

import numpy as np
import matplotlib
import sys

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
                    (xmin, ymin, dx, dy, nnox, nnoy) = tuple(lin.strip().split(';'))
                    sol = np.zeros((int(nnoy), int(nnox)))
                    continue
                if (lin.endswith(';')):
                    lin = lin[:-1]
                lst = lin.split(';')
                sol[linecount] = lst
                linecount += 1

            xmax = float(float(xmin)+(float(dx)*float(float(nnox)-1)))
            ymax = float(float(ymin)+(float(dy)*float(float(nnoy)-1)))

            x = np.arange(float(xmin),float(xmax),float((float(xmax) - float(xmin)) / float(nnox)))
            y = np.arange(float(ymin),float(ymax),float((float(ymax) - float(ymin)) / float(nnoy)))
            X, Y = np.meshgrid(x, y)

            fig = plt.figure(figsize=(20, 20))
            
            ax = plt.axes(projection='3d')
            ax.set_xlabel('X axis')
            ax.set_ylabel('Y axis')
            ax.plot_surface(X, Y, sol, cmap='viridis', edgecolor='none')

            plt.savefig('plot.jpg', dpi=300, bbox_inches='tight')
            plt.show()

    except IndexError as ie:
        print("Index error({0}): {1}".format(ie.errno, ie.strerror))
    except IOError as e:
        print("I/O error({0}): {1}".format(e.errno, e.strerror))
    except TypeError as w:
        print("Type error")
    except Exception:
        print(" Some exception ocurred ", sys.exc_info()[0])
        # os.system("ls -l")


if __name__ == '__main__':

    plot()
