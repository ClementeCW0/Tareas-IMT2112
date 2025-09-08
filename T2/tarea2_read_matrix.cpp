// Elwin van 't Wout
// Pontificia Universidad Católica de Chile
// 27-8-2025

// Read a matrix from a text file and store specific lines in an array.

#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <mpi.h>
using namespace std;

//double matrix_dimensions(ifstream* file)
//{
//}
double* submatrix(ifstream* file, int nrows, int ncols, int my_firstrow, int my_nrows, int rank)
{
    //int nrows, ncols;
    double *my_matrix;
    double tmp;

    //ifstream file;

    //file.open("matrix.txt");

    //if (*file.is_open())
    //{
        //*file >> nrows;
        //cout << "Number of rows: " << nrows << endl;

        //*file >> ncols;
        //cout << "Number of columns: " << ncols << endl;

        //int my_firstrow = 2;
        //int my_nrows = 3;
        cout << "Rank " << rank << " " << "Read " << my_nrows << " rows starting from row " << my_firstrow << endl;

        my_matrix = new double[my_nrows * ncols];

        for (int i=0; i<(my_firstrow)*ncols; i++) {
            *file >> tmp;
            //cout << "Rank " << rank << " " << "skipped: " << tmp << endl;
        }

        cout << "Rank " << rank << " " << "Store matrix elements" << endl;
        for (int i=0; i<my_nrows*ncols; i++) {
            *file >> my_matrix[i];
            cout << "Rank " << rank << " " << i << " " << my_matrix[i] << endl;
        }

        //*file.close();
    //}
    //else
    //{
    //    cout << "Unable to open file." << endl;
    //}
	return my_matrix;
}
	double* partial_matvec(double* mat, double* vec, int rows, int cols, int start_col){
		double* result;
		if (sizeof(mat)/sizeof(mat[0]) == rows * cols && sizeof(vec)/sizeof(vec[0]) == cols ) {
			  
		}
		else {
			throw invalid_argument("Invalid mat-vec multiplication");
		}
		return result;

}

int main(){
	MPI_Init(NULL,NULL);
	int world_size, world_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    	ifstream file;

    	file.open("matrix.txt");
	double *A_ij;
	int nrows, ncols;
	int my_firstrow, my_nrows;
    	if (file.is_open()){
        	file >> nrows;
        	cout << "Number of rows: " << nrows << endl;

        	file >> ncols;
        	cout << "Number of columns: " << ncols << endl;

		my_nrows = nrows / world_size;
		my_firstrow = world_rank * my_nrows;
		if (world_rank == 0) {
			cout << "I'm the first guy" << endl;
		} else if (world_rank == world_size - 1){
			cout << "I'm the last guy" << endl;
			my_nrows += nrows % world_size;
			}
		A_ij = submatrix(&file, nrows, ncols, my_firstrow, my_nrows, world_rank);
        	file.close();
    	}
    	else
    	{
    	    cout << "Unable to open file." << endl;
    	}

	double *b;
	b = new double[my_nrows];
	for (int i = 0; i < my_nrows; i++){
		b[i] = 1;
	}
	int n_iterations = 10;
	// MAIN ITERATION LOOP:
	for (int k = 0; k < n_iterations; k++){
		double *buffer;

		MPI_Gather(&b, MPI_DOUBLE, );

	}
	delete[] A_ij;
	MPI_Finalize();
	return 0;
}
