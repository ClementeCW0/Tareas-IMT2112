import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

df = pd.read_csv("times.csv", header=None)

Y = df.iloc[0]
print(len(Y))
X = np.array([i for i in range(1, len(Y) + 1)])

plt.plot(X, Y)
plt.show()
