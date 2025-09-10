// Elwin van 't Wout
// Pontificia Universidad Católica de Chile
// 27-8-2025

// Read a matrix from a text file and store specific lines in an array.

#include <iostream>
#include <cmath>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <chrono>
#include <mpi.h>
using namespace std;

//double matrix_dimensions(ifstream* file)
//{
//}
double* submatrix(ifstream* file, int nrows, int ncols, int my_firstrow, int my_nrows, int rank)
{
    double *my_matrix;
    double tmp;
        my_matrix = new double[my_nrows * ncols];

        for (int i=0; i<(my_firstrow)*ncols; i++) {
            *file >> tmp;
            //cout << "Rank " << rank << " " << "skipped: " << tmp << endl;
        }
        //cout << "Rank " << rank << " " << "Store matrix elements" << endl;
        for (int i=0; i<my_nrows*ncols; i++) {
            *file >> my_matrix[i];
            //cout << "Rank " << rank << " " << i << " " << my_matrix[i] << endl;
        }
	return my_matrix;
}

int main(){
	using chrono::high_resolution_clock;

	MPI_Init(NULL,NULL);
	int world_size, world_rank, proc_len;
	char proc_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Get_processor_name(proc_name, &proc_len);
	cout << "Process " << world_rank << " uses processor " << proc_name << endl;

    	ifstream file;

    	file.open("matrix.txt");
	double* A_ij;
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

	double* local_b = new double[my_nrows];
	double* full_b = new double[ncols];
	for (int i = 0; i < my_nrows; i++){
		local_b[i] = 1;
	}
	int n_iterations = 3000;
	// MAIN ITERATION LOOP:
	for (int k = 0; k < n_iterations; k++){
		//cout << "Iteration: " << k << endl;
		MPI_Allgatherv(local_b, my_nrows, MPI_DOUBLE, full_b, allgthr_count, allgthr_displacement, MPI_DOUBLE, MPI_COMM_WORLD);
		//cout << "Iteration " << k << ": " << " My rank: " << world_rank << endl;
		for (int c = 0; c < ncols; c++){
			//cout << "(" << k << ", " << world_rank << ", " << c << ") " << full_b[c] << endl;;
		}

		// Compute local matvec in each process
        	double* new_b = new double[my_nrows]();
		for (int r = 0; r < my_nrows; r++){
			for (int c = 0; c < ncols; c++){
      				new_b[r] += A_ij[r * ncols + c] * full_b[c];
      			}
		}
		//Compute local module of Ab doing
		double local_mod = 0;
		for (int i = 0; i < my_nrows; i++){
			local_mod += new_b[i] * new_b[i];
		}
		double full_mod = 0;
		MPI_Allreduce(&local_mod, &full_mod, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		
		// Compute the local summand of the eigenvalue approximation
		double local_eigen_max = 0;
		for (int i = 0; i < my_nrows; i++){
			local_eigen_max += new_b[i] * local_b[i];
		}
		double full_eigen_max = 0;
		MPI_Reduce(&local_eigen_max, &full_eigen_max, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		if (world_rank == 0 && k % 100 == 0){
			cout << full_eigen_max << endl;
		}

		// Update local value
		for (int i = 0; i < my_nrows; i++){
			local_b[i] = new_b[i] / sqrt(full_mod);
		}
		delete[] new_b;

	}
	delete[] A_ij;
	delete[] local_b;
	delete[] full_b;
	MPI_Finalize();
	return 0;
}
