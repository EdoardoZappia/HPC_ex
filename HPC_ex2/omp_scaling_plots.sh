#!/bin/bash
#SBATCH --job-name=HPC-Weak-Scaling
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=24
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

# Loop through different problem sizes
for width in 800 1600 3200; do
    height=$((width * 3 / 4))  # Maintain aspect ratio
    echo "Running with $width x $height resolution."
    export OMP_NUM_THREADS=$width  # Adjust number of threads with problem size
    mpirun -np 1 ./mandelbrot_plots $width $height -2.0 -1.0 1.0 1.0 255
done

