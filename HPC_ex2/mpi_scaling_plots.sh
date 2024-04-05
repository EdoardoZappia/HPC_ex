#!/bin/bash
#SBATCH --job-name=HPC-Strong-Scaling
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=24
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

for total_tasks in 24 48 96 192; do
    echo "Running with $total_tasks MPI tasks."
    mpirun -np $total_tasks ./mandelbrot_plots 800 600 -2.0 -1.0 1.0 1.0 255
done

