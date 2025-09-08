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
int* submatrix(ifstream* file, int nrows, int ncols, int my_firstrow, int my_nrows, int rank)
{
    //int nrows, ncols;
    int *my_matrix;
    int tmp;

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
        //cout << "Rank " << rank << " " << "Read " << my_nrows << " rows starting from row " << my_firstrow << endl;

        my_matrix = new int[my_nrows * ncols];

        for (int i=0; i<(my_firstrow)*ncols; i++) {
            *file >> tmp;
            //cout << "Rank " << rank << " " << "skipped: " << tmp << endl;
        }

        //cout << "Rank " << rank << " " << "Store matrix elements" << endl;
        for (int i=0; i<my_nrows*ncols; i++) {
            *file >> my_matrix[i];
            //cout << "Rank " << rank << " " << i << " " << my_matrix[i] << endl;
        }

        //*file.close();
    //}
    //else
    //{
    //    cout << "Unable to open file." << endl;
    //}
	return my_matrix;
}
	int* partial_matvec(int* mat, int* vec, int rows, int cols, int rank){
		cout << "Rank " << rank << ": " << sizeof(vec)/sizeof(int) << " " << cols << endl;
		int* result;
		//if (size(vec) == cols ) {
        		result = (int*)malloc(sizeof(int) * rows);
			for (int r = 0; r < rows; r++){
				for (int c = 0; c < cols; c++){
					result[r] += mat[r * cols + c] * vec[c];
				}
			}
		//}
		//else {
		//	throw invalid_argument("Invalid mat-vec multiplication");
		//}
		return result;

}

int main(){
	MPI_Init(NULL,NULL);
	int world_size, world_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    	ifstream file;

    	file.open("matrix.txt");
	int *A_ij;
	int nrows, ncols, allgthr_count[world_size], allgthr_displacement[world_size];
	int my_firstrow, my_nrows;
    	if (file.is_open()){
        	file >> nrows;
        	//cout << "Number of rows: " << nrows << endl;

        	file >> ncols;
        	//cout << "Number of columns: " << ncols << endl;

		my_nrows = nrows / world_size;
		my_firstrow = world_rank * my_nrows;
		// Define the size of the vectors to be sent in the allgather operation.
		for (int p = 0; p < world_size; p++){
			// Number of entries in b_i for the ith process
			allgthr_count[p] = my_nrows;
			allgthr_displacement[p] = my_nrows * p;
			if (p == world_size - 1){
				// Add remaining rows to the last process
				allgthr_count[p] += nrows % world_size;
			}

		}
		if (world_rank == 0) {
			//cout << "I'm the first guy" << endl;
		} else if (world_rank == world_size - 1){
			//cout << "I'm the last guy" << endl;
			my_nrows += nrows % world_size;
			}
		A_ij = submatrix(&file, nrows, ncols, my_firstrow, my_nrows, world_rank);
        	file.close();
    	}
    	else
    	{
		cout << "Unable to open file." << endl;
		return 1;
    	}

	int* b;
        b = (int*)malloc(sizeof(int) * my_nrows);
	for (int i = 0; i < my_nrows; i++){
		b[i] = 1;
	}
	int n_iterations = 3;
	// MAIN ITERATION LOOP:
	for (int k = 0; k < n_iterations; k++){
		//cout << "Iteration: " << k << endl;
		int buffer[ncols];

		MPI_Allgatherv(b, my_nrows, MPI_INT, buffer, allgthr_count, allgthr_displacement, MPI_INT, MPI_COMM_WORLD);
		cout << "Iteration " << k << ": " << " My rank: " << world_rank << endl;
		for (int c = 0; c < ncols; c++){
			cout << buffer[c] << endl;;
		}

		//b = partial_matvec(A_ij, buffer, my_nrows, ncols, world_rank);
        	b = (int*)calloc(my_nrows, sizeof(int));
		for (int r = 0; r < my_nrows; r++){
			for (int c = 0; c < ncols; c++){
      				b[r] += A_ij[r * ncols + c] * buffer[c];
      			}
		}
	}
	delete[] b;
	delete[] A_ij;
	MPI_Finalize();
	return 0;
}
