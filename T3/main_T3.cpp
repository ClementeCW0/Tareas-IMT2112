#include <cmath>
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
float tidy_alpha(int i, int j, int n, int k){
	// Función para generar de forma ordenada los valores del stencil.
	switch (k) {
		case 0: // NORTE
		return - n * n * alpha(i / n, (j + 0.5) / n);
		break;

		case 1: // SUR
		return - n * n * alpha(i / n, (j - 0.5) / n);
		break;

		case 2: // ESTE
		return - n * n * alpha((i + 0.5) / n, j / n);
		break;

		case 3: // OESTE
		return - n * n * alpha((i - 0.5) / n, j / n);
		break;

		case 4: // CENTRO
		return - n * n * (
				  alpha((i - 0.5) / n, j / n) + \
				  alpha((i + 0.5) / n, j / n) + \
				  alpha(i / n, (j + 0.5) / n) + \
				  alpha(i / n, (j - 0.5) / n) + \
				 ) + 1
		break;


	}
}
float* get_stencil(int num_dividionvs) {
	// Se asume que el stencil es de cruz con 5 elementos. Se creará un arreglo de tamaño
	// 5 * num_divisoins \times num_divisions que representará los cinco arreglos del stencil
	// en el orden [N S E W C]
	float* stencil = (float*) calloc(5 * num_dividions * num_dividions, sizeof(float));
	for (int k = 0; k < 5; k++){
		for (int i = 0; i < num_dividionvs; i++){
			for (int j = 0; j < num_dividionvs; j++)

		}
	}



	return stencil;
}
float* sparse_matvec(int n, int* offset, float* vec) {
	float* result = (float*) calloc(n, sizeof(float));
	#pragma omp parallel for
	for (int i = 0; i < n * n; i++) {
		}
	}
	return result;
}
float* sparse_matvec(int n){
	float* mat = (float*) calloc(n, sizeof(float));


	return mat;
}

int main(){
	int N = 100;
	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		int tid = omp_get_thread_num();
		cout << tid << " " << i << "\n";
	}
	return 0;
}
