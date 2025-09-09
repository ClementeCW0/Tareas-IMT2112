# Elwin van 't Wout
# Pontificia Universidad Catolica de Chile
# 27-8-2025

# Generate a matrix with real eigenvalues uniformly located in a specific
# range and store the matrix in a text file.

import numpy as np

def generate_matrix(dim, seed=0):
    rng = np.random.default_rng(seed)

    # Unscipyfication of the code (the cluster doesn't have scipy)
    # Random Gaussian matrix
    X = rng.normal(size=(dim, dim))
    # QR decomposition
    Q, R = np.linalg.qr(X)
    # Fix sign ambiguity so Q is uniformly distributed over O(n)
    Q *= np.sign(np.diag(R))
    # Eigenvalues from 1 to 10
    b = np.linspace(1., 10., dim)
    # Construct symmetric matrix



    #from scipy.stats import ortho_group
    #from scipy.sparse import spdiags
    #a = ortho_group.rvs(dim, random_state=0)
    #b = np.linspace(1., 10., dim)
    #return a @ spdiags(b, 0, dim, dim) @ a.T
    return Q @ np.diag(b) @ Q.T

def write_matrix(matrix, filename='matrix.txt'):
    with open(filename, 'w') as f:
        f.write(str(mat.shape[0])+"\n"+str(mat.shape[1])+"\n")
        for i in range(mat.shape[0]):
            for j in range(mat.shape[1]):
                f.write(str(matrix[i,j])+"\n")
        f.write("end")

ndim = 1000
mat = generate_matrix(ndim)
write_matrix(mat)
