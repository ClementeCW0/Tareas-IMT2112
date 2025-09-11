#!/bin/bash

# Nombre del trabajo
#SBATCH --job-name=IMT2112
# Archivo de salida
#SBATCH --output=logs/output_%j.txt
# Cola de trabajo
#SBATCH --partition=full
# Solicitud de cpus
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1

echo "start script"
date
mpic++ -std=c++11 tarea2_read_matrix.cpp
for i in {1..32}
do
	echo "processors: $i"
	mpirun -np $i ./a.out
done
echo "end script"
date

