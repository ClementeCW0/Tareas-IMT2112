import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv("iteration_data.csv", header=None)
with open("data") as data_file:
    Nx = int(data_file.readline())
    Ny = int(data_file.readline())

for row in df.iloc:
    x = np.linspace(0,1,Nx)
    y = np.linspace(0,1,Ny)
    X, Y = np.meshgrid(y, x)
    Z = np.array(row[:-1]).reshape(Nx, Ny)

    ax = plt.figure().add_subplot(projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis')
    plt.show()
