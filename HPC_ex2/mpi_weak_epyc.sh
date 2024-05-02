#!/bin/bash
#SBATCH --job-name=HPC-MPI-WeakScaling
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=128
#SBATCH --time=02:00:00
#SBATCH --partition=EPYC
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

export OMP_NUM_THREADS=1

OUTPUT_CSV="mpi_weak_epyc.csv"
echo "MPI_Processes,Columns,Rows,Execution_Time" > $OUTPUT_CSV

BASE_COLS=1000
BASE_ROWS=1000

for total_procs in {2..256..2}; do
    let cols=BASE_COLS
    let rows=$((BASE_ROWS*total_procs))
    execution_time=$(mpirun -np $total_procs ./mandelbrot $cols $rows -2.0 -1.0 1.0 1.0 255 $OMP_NUM_THREADS)
    
    echo "$total_procs,$cols,$rows,$execution_time" >> $OUTPUT_CSV
done