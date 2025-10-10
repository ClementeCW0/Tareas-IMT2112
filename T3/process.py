import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv("iteration_data.csv", header=None)
with open("data") as data_file:
    Nx = int(data_file.readline())
    Ny = int(data_file.readline())

count = 1
for row in [df.iloc[i] for i in [0, 10, -1]]:
    x = np.linspace(0,1,Nx)
    y = np.linspace(0,1,Ny)
    X, Y = np.meshgrid(y, x)
    Z = np.array(row[:-1]).reshape(Nx, Ny)
    
    ax = plt.figure().add_subplot(projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis')
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    plt.show()

