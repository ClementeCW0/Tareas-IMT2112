#!/bin/bash

# Nombre del trabajo
#SBATCH --job-name=w_scaling
# Archivo de salida
#SBATCH --output=logs/output_%j.txt
# Cola de trabajo
#SBATCH --partition=full
# Solicitud de cpus
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1

echo "start script"
date
mpic++ -std=c++11 tarea2_read_matrix.cpp
for i in {1..25}
do
	ndim=$((200*$i))
	python tarea2_generate_matrix.py $ndim &&
	mpirun -np 8 ./a.out
done
echo "end script"
date

