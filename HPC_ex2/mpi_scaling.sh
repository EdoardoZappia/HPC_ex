#!/bin/bash
#SBATCH --job-name=HPC-MPI-Scaling
#SBATCH --nodes=4  # Massimo numero di nodi che vuoi testare
#SBATCH --ntasks-per-node=24  # Adatta questo al massimo numero di task per nodo
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

export OMP_NUM_THREADS=1

# Loop attraverso un numero variabile di task MPI totali
for total_tasks in 24 48 96; do
    echo "Running with $total_tasks MPI tasks."
    mpirun -np $total_tasks ./mandelbrot 800 600 -2.0 -1.0 1.0 1.0 255
done
