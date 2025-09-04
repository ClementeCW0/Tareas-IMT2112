#include "submatrix.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(){
	int ndim = 2;
	double* A_ij;
	A_ij = submatrix(3, ndim, 4, ndim);
	for (int c = 0; c < ndim * ndim; c++){
		cout << c << " " << A_ij[c] << endl;
	}
	delete[] A_ij;
	return 0;
}
