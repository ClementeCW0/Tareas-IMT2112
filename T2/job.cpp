#include "tarea2_read_matrix.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(){
	int my_firstcol = 2;
	int my_ncols = 2;
	double* A_ij;
	A_ij = submatrix(my_firstcol, my_ncols);

	for (int c = 0; c < my_firstcol * my_ncols; c++){
		cout << c << " " << A_ij[c] << endl;
	}
	delete[] A_ij;
	return 0;
}
