#include <omp.h>
#include <iostream>
using namespace std;

float* random_matrix(int n){
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
