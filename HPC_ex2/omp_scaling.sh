#!/bin/bash
#SBATCH --job-name=HPC-OMP-Scaling
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=24  # Assicurati che questo corrisponda al numero massimo di thread OMP che vuoi testare
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

for OMP_NUM_THREADS in 1 2 4 8 16 24; do
    export OMP_NUM_THREADS
    echo "Running with $OMP_NUM_THREADS OpenMP threads."
    mpirun ./mandelbrot 800 600 -2.0 -1.0 1.0 1.0 255
done
