#!/bin/bash
#SBATCH --job-name=HPC-MPI-Scaling
#SBATCH --nodes=4  # Massimo numero di nodi che vuoi testare
#SBATCH --ntasks-per-node=24  # Adatta questo al massimo numero di task per nodo
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

export OMP_NUM_THREADS=1

# Nome del file CSV per salvare i tempi di esecuzione
output_file="execution_times_mpi.csv"

# Creiamo l'intestazione del file CSV
echo "Total Tasks,Execution Time (s)" > $output_file

# Loop attraverso un numero variabile di task MPI totali
for total_tasks in {2..96..2}; do
    echo "Running with $total_tasks MPI tasks."
    
    # Esegui mpirun e cattura il tempo di esecuzione
    mpirun -np $total_tasks ./mandelbrot 800 600 -2.0 -1.0 1.0 1.0 255
    
    # Aggiungi i dati al file CSV
    execution_time=$(<temp_execution_time.txt)
    echo "$total_tasks,$execution_time" >> $output_file
    rm temp_execution_time.txt # Rimuovi il file temporaneo
done

