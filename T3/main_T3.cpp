// Compilar con g++ -o tarea_3 main_T3.cpp -fopenmp
#include <cmath>
#include <fstream>
#include <omp.h>
#include <iostream>
using namespace std;

double alpha(double x, double y){
	return x * (x - 1) * y * (y - 1) + 1;
}
double f(double x, double y){
	double cx = 0.4;
	double cy = 0.8;
	double sx = 0.2;
	double sy = 0.1;
	return exp(-pow((x - cx) / sx, 2.0) / 2 -pow((y - cy) / sy, 2.0) / 2) / (2 * M_PI * sx * sy);
}
double tidy_alpha(int i, int j, int nx, int ny, int k){
	// Función para generar de forma ordenada los valores del stencil.
	double result;
	switch (k) {
		case 0: // NORTE
			result =  - nx * ny * alpha((double) i / (double) nx, (double) (j + 0.5) / (double) ny);
			break;

		case 1: // SUR
			result =  - nx * ny * alpha((double) i / (double) nx, (double) (j - 0.5) / (double) ny);
			break;

		case 2: // ESTE
			result =  - nx * ny * alpha((double) (i + 0.5) / (double) nx, (double) j / (double) ny);
			break;

		case 3: // OESTE
			result =  - nx * ny * alpha((double) (i - 0.5) / (double) nx, (double) j / (double) ny);
			break;

		case 4: // CENTRO
			result =  nx * ny * (
				  alpha((i - 0.5) / (double) nx, (double) j / (double) ny) + \
				  alpha((i + 0.5) / (double) nx, (double) j / (double) ny) + \
				  alpha((double) i / (double) nx, (double) (j + 0.5) / (double) ny) + \
				  alpha((double) i / (double) nx, (double) (j - 0.5) / (double) ny)
				 ) + 1.0;
			break;
		case 5:
			// Vector b. Si estamos en el interior de (0,1) toma el valor de f, si
			// estamos en el borde es cero.
			if (i != 0 && i != ny - 1 && j != 0 && j != nx - 1){
				result = f((double) i / (double) ny, (double) j / (double) nx);
			} else {
				result = 0;
			}
			break;
	}
	return result;
}
double* get_stencil(int Nx, int Ny) {
	// Se creará un arreglo de tamaño 6 * Nx \times Ny que representará los cinco
	// arreglos del stencil en el orden [N S E W C] junto con el vector b al final.
	double* stencil = (double*) calloc(6 * Nx * Ny, sizeof(double));
	for (int k = 0; k < 6; k++){
		// Es mejor paralelizar el loop interior, dado que el primer loop itera solo sobre
		// 6 elementos.
		#pragma omp parallel for
		for (int j = 0; j < Nx; j++){
			for (int i = 0; i < Ny; i++){
				stencil[k * Nx * Ny + Nx * j + i] = tidy_alpha(i, j, Nx, Ny, k);
			}
		}
	}
	return stencil;
}
double* sparse_matvec(int Nx, int Ny, double* stencil, double* vec) {
	// Implementado solo para stencils de cruz con 5 elementos
	double* result = (double*) calloc(Nx * Ny, sizeof(double));
	double N, S, E, W, C; 
	#pragma omp parallel for 
	for (int i = 0; i < Ny; i++) {
		for (int j = 0; j < Nx; j++){
			N = 0; S = 0; E = 0; W = 0;
			// Debemos ajustar el stencil según el lugar de la grilla en la que nos encontramos
			if (j != Ny - 1){ //NORTE
				N = stencil[Nx * j + i] * vec[Nx * (j + 1) + i];
			}
			if (j != 0){ //SUR
				S = stencil[Nx * Ny + Nx * j + i] * vec[Nx * (j - 1) + i];
			}
			if (i != Nx - 1){ //ESTE
				E = stencil[2 * Nx * Ny + Nx * j + i] * vec[Nx * j + i + 1];
			}
			if (i != 0){ //OESTE
				W = stencil[3 * Nx * Ny + Nx * j + i] * vec[Nx * j + i - 1];
			}
			// CENTRO
			C = stencil[4 * Nx * Ny + Nx * j + i] * vec[Nx * j + i];
			result[Nx * j + i] = N + S + E + W + C;
		}
	}
	return result;
}

double dot(double* vec1, double* vec2, int size){
	double result = 0;
	#pragma omp parallel for reduction(+:result)
	for (int i = 0; i < size; i++){
		result += vec1[i] * vec2[i];
	}
	return result;
}

int main(){
	ifstream data;
	// El Archivo data fue implementado para no tener que compilar nuevamente el código si
	// se cambia el tamaño de la grilla o el número de iteraciones. Los contenidos de este
	// archivo son:
	// Nx
	// Ny
	// Número de Iteraciones
	// Número de Threads
	data.open("data");
	int Nx, Ny, max_iterations, num_threads;
	data >> Nx;
	data >> Ny;
	data >> max_iterations;
	data >> num_threads;
	data.close();
	omp_set_num_threads(num_threads);
	double* stencil = get_stencil(Nx, Ny);
	int g_size = Nx * Ny; // grid size
	double* x = (double*) calloc(g_size, sizeof(double));
	double* p = (double*) calloc(g_size, sizeof(double));
	double* q = (double*) calloc(g_size, sizeof(double));
	double* r = sparse_matvec(Nx, Ny, stencil, x);
	double rho, new_rho, beta, delta;
	double* b = (double*) calloc(g_size, sizeof(double));
	#pragma omp parallel for
	for (int j = 0; j < Nx; j++){
		for (int i = 0; i < Ny; i++){
			b[Nx * j + i] = stencil[5 * g_size + Nx * j + i];
		}
	}
	#pragma omp parallel for
	for (int k = 0; k < g_size; k++){
		r[k] = r[k] - b[k];
	}

	ofstream xdata;
	xdata.open("iteration_data.csv");
	///////// MAIN LOOP /////////////////////////////
	for (int i = 1; i < max_iterations + 1; i++){
		cout << "--------ITERATION " << i << " ---------- ";
		new_rho = dot(r, r, g_size);
		if (i == 1){
			for (int k = 0; k < g_size; k++){
				p[k] = r[k];
			}
			rho = new_rho;
		} else {
			beta = new_rho/rho;
			double p_check = 0;
			for (int k = 0; k < g_size; k++){
				p[k] = r[k] + beta * p[k];
			}
		}
		q = sparse_matvec(Nx, Ny, stencil, p);
		delta = new_rho / dot(p, q, g_size);
		#pragma omp parallel for
		for (int k = 0; k < g_size; k++) {
			x[k] = x[k] - delta * p[k];
			xdata << x[k] << ",";
			
		}
		#pragma omp parallel for
		for (int k = 0; k < g_size; k++) {
			r[k] = r[k] - delta * q[k];
		}
		cout << "res: " << dot(r, r, g_size) << "\n";
		xdata << "\n";
		rho = new_rho;
	}
	xdata.close();
	free(stencil);
	free(b);
	free(r);
	free(p);
	free(q);
	free(x);
	return 0;
}
