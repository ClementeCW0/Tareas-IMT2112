// Compilar con g++ -o tarea_3 main_T3.cpp -fopenmp
#include <cmath>
#include <fstream>
#include <omp.h>
#include <iostream>
using namespace std;

float alpha(float x, float y){
	return x * (x - 1) * y * (y - 1) + 1;
}
float f(float x, float y){
	float cx = 0.4;
	float cy = 0.8;
	float sx = 0.2;
	float sy = 0.1;
	return exp(-pow((x - cx) / sx, 2.0) / 2 -pow((y - cy) / sy, 2.0) / 2) / (2 * M_PI * sx * sy);
}
float tidy_alpha(int i, int j, int nx, int ny, int k){
	// Función para generar de forma ordenada los valores del stencil.
	float result;
	switch (k) {
		case 0: // NORTE
			result =  - nx * ny * alpha(i / (float) nx, (j + 0.5) / (float) ny);
			break;

		case 1: // SUR
			result =  - nx * ny * alpha(i / (float) nx, (j - 0.5) / (float) ny);
			break;

		case 2: // ESTE
			result =  - nx * ny * alpha((i + 0.5) / (float) nx, j / (float) ny);
			break;

		case 3: // OESTE
			result =  - nx * ny * alpha((i - 0.5) / (float) nx, j / (float) ny);
			break;

		case 4: // CENTRO
			result =  nx * ny * (
				  alpha((i - 0.5) / (float) nx, j / (float) ny) + \
				  alpha((i + 0.5) / (float) nx, j / (float) ny) + \
				  alpha(i / (float) nx, (j + 0.5) / (float) ny) + \
				  alpha(i / (float) nx, (j - 0.5) / (float) ny)
				 ) + 1;
			break;
		case 5:
			result = f((float) i / (float) nx, (float) j / (float) ny);
			break;
	}
	return result;
}
float* get_stencil(int Nx, int Ny) {
	// Se creará un arreglo de tamaño 6 * Nx \times Ny que representará los cinco
	// arreglos del stencil en el orden [N S E W C] junto con el vector b al final.
	float* stencil = (float*) calloc(6 * Nx * Ny, sizeof(float));
	for (int k = 0; k < 6; k++){
		for (int i = 0; i < Nx; i++){
			for (int j = 0; j < Ny; j++){
				if (isnan(tidy_alpha(i, j, Nx, Ny, k))){
					cout << "stencil: " << "(" << i << ", " << j << ") " << tidy_alpha(i, j, Nx, Ny, k) << "\n";
				}
				stencil[k * Nx * Ny + Nx * i + j] = tidy_alpha(i, j, Nx, Ny, k);
			}
		}
	}
	return stencil;
}
float* sparse_matvec(int Nx, int Ny, float* stencil, float* vec) {
	// Implementado solo para stencils de cruz con 5 elementos
	float* result = (float*) calloc(Nx * Ny, sizeof(float));
	float N, S, E, W, C; 
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++){
			N = 0; S = 0; E = 0; W = 0;
			// Debemos ajustar el stencil según el lugar de la grilla en la que nos encontramos
			if (j != 0){
				W = stencil[3 * Nx * Ny + Nx * i + j] * vec[Nx * i + j - 1];
			}
			if (j != Nx - 1){
				E = stencil[2 * Nx * Ny + Nx * i + j] * vec[Nx * i + j + 1];
			}
			if (i != 0){
				N = stencil[Nx * i + j] * vec[Nx * (i - 1) + j];
			}
			if (i != Ny - 1){
				S = stencil[Nx * Ny + Nx * i + j] * vec[Nx * (i + 1) + j];
			}
			C = stencil[4 * Nx * Ny + Nx * i + j] * vec[Nx * i + j];
			result[Nx * i + j] = N + S + E + W + C;
		}
	}
	return result;
}
float dot(float* vec1, float* vec2, int size){
	float result = 0;
	#pragma omp parallel for reduction(+:result)
	for (int i = 0; i < size; i++){
		//cout << vec1[i] << " " << vec2[i] << "\n";
		result += vec1[i] * vec2[i];
	}
	return result;
}
int main(){
	ifstream data;
	data.open("data");
	int Nx, Ny, max_iterations;
	data >> Nx;
	data >> Ny;
	data >> max_iterations;
	data.close();
	float* stencil = get_stencil(Nx, Ny);
	int g_size = Nx * Ny; // grid size
	float* x = (float*) calloc(g_size, sizeof(float));
	// Si x es el vector 0, el valor de delta queda indefinido en la primera iteración
	// dado que b es el vector 0.
	//for (int i = 0; i < g_size; i++){
	//	x[i] = 1.0f;
	//}
	float* p = (float*) calloc(g_size, sizeof(float));
	float* q = (float*) calloc(g_size, sizeof(float));
	float* r = (float*) calloc(g_size, sizeof(float));
	r = sparse_matvec(Nx, Ny, stencil, x);
	float rho, new_rho, beta, delta;
	float* b = (float*) calloc(g_size, sizeof(float));
	for (int i = 0; i < Nx; i++){
		for (int j = 0; j < Ny; j++){
			b[Nx * i + j] = stencil[5 * g_size + Nx * i + j];
		}
	}
	for (int k = 0; k < g_size; k++){
		if (isnan(r[k])){
			cout << "nan r entry: " << k << "\n";
		}
		if (isnan(b[k])){
			cout << "nan b entry: " << k << "\n";
		}
		r[k] = r[k] - b[k];
		if (isnan(r[k])){
			cout << "nan r entry: " << k << "\n";
		}
	}

	ofstream xdata;
	xdata.open("iteration_data.csv");
	///////// MAIN LOOP /////////////////////////////
	for (int i = 1; i < max_iterations + 1; i++){
		cout << "--------ITERATION " << i << "----------\n";
		new_rho = dot(r, r, g_size);
		if (isnan(new_rho)) {
			cout << "nan rho\n";
		} else {
			cout << "rho: " << new_rho << "\n";
		}
		if (i == 1){
			for (int k = 0; k < g_size; k++){
				p[k] = r[k];
				if (isnan(p[k])){
					cout << "nan p entry: " << k << "\n"; 
				}
			}
			rho = new_rho;
		} else {
			beta = new_rho/rho;
			if (isnan(beta)){
				cout << "nan beta: " << "(" << i << ") " << "\n";
			}
			float p_check = 0;
			for (int k = 0; k < g_size; k++){
				p[k] = r[k] + beta * p[k];
				if (isnan(p[k])) {
					//cout << "nan p entry: " << k << "\n";
					p_check += 1;
				}
			}
			cout << "nan in p: " << p_check / (float) g_size << "\n";
		}
		float* new_q = sparse_matvec(Nx, Ny, stencil, p);
		//free(q);
		for (int k = 0; k < g_size; k++) {
			q[k] = new_q[k];
		}
		delta = rho / dot(p, q, g_size);
		cout << "delta: " << delta << "\n";
		for (int k = 0; k < g_size; k++) {
			x[k] = x[k] - delta * p[k];
			xdata << x[k] << ",";
			
		}
		for (int k = 0; k < g_size; k++) {
			r[k] = r[k] - delta * q[k];
		}
		cout << i << " " << dot(r, r, g_size) << "\n";
		xdata << "\n";

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
