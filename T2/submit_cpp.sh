#!/bin/bash

# Nombre del trabajo
#SBATCH --job-name=IMT2112
# Archivo de salida
#SBATCH --output=data.txt
# Cola de trabajo
#SBATCH --partition=full
# Solicitud de cpus
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1

echo "start script"
date
mpic++ -std=c++11 tarea2_read_matrix.cpp
for p in {1..5}
do
	mpirun -np &p a.out
done
echo "end script"
date

