// Elwin van 't Wout
// Pontificia Universidad Católica de Chile
// 27-8-2025

// Read a matrix from a text file and store specific lines in an array.

#include <iostream>
#include <fstream>
using namespace std;

double* submatrix(int my_firstrow, int my_nrows, int my_firstcol, int my_ncols)
{
    int nrows, ncols;
    double *my_matrix;
    double tmp;

    ifstream file;

    file.open("matrix.txt");

    if (file.is_open())
    {
        file >> nrows;
        //cout << "Number of rows: " << nrows << endl;

        file >> ncols;
        //cout << "Number of columns: " << ncols << endl;

        //int my_firstrow = 2;
        //int my_nrows = 3;
        //cout << "Read " << my_nrows << " rows starting from row " << my_firstrow << endl;

        my_matrix = new double [my_nrows * ncols];

        for (int i=0; i<(my_firstrow)*ncols; i++) {
            file >> tmp;
            cout << "skipped: " << tmp << endl;
        }

        //cout << "Store matrix elements" << endl;
	int submatrix_index = 0;
        for (int i=0; i<my_nrows*ncols; i++) {
			bool not_in_col = true;
			for (int j = my_firstcol; j < my_firstcol + my_ncols; j++) {
	     			if (i % ncols == j){
	     				file >> my_matrix[submatrix_index];
            				//cout << i << " " << my_matrix[submatrix_index] << endl;
					not_in_col = false;
					submatrix_index++;
					break;
	     			}
	     		}
			if (not_in_col) {
		                 file >> tmp;
            	                 //cout << "skipped: " << tmp << endl;
			}
        }

        file.close();
    }
    else
    {
        cout << "Unable to open file." << endl;
    }
	return my_matrix;
}


//int main()
//{
//	int ndim = 2;
//	double* A_ij;
//	A_ij = submatrix(3, ndim, 4, ndim);
//	for (int c = 0; c < ndim * ndim; c++){
//		cout << c << " " << A_ij[c] << endl;
//	}
//	delete[] A_ij;
//	return 0;
//}
