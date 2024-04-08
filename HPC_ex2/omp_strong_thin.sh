#!/bin/bash
#SBATCH --job-name=HPC-OMP-Scaling
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=24  # Assicurati che questo corrisponda al numero massimo di thread OMP che vuoi testare
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

# Definisci il nome del file CSV per salvare i risultati
OUTPUT_CSV="execution_times_omp.csv"

# Inizializza il file CSV con l'intestazione se non esiste
echo "OMP_NUM_THREADS,Execution_Time" > $OUTPUT_CSV

for OMP_NUM_THREADS in {1..24..1}; do
    export OMP_NUM_THREADS
    echo "Running with $OMP_NUM_THREADS OpenMP threads."
    
    # Esegui il programma e salva il tempo di esecuzione
    mpirun ./mandelbrot 800 600 -2.0 -1.0 1.0 1.0 255

    # Aggiungi i dati al file CSV
    execution_time=$(<temp_execution_time.txt)
    
    # Salva il tempo di esecuzione nel file CSV
    echo "$OMP_NUM_THREADS,$execution_time" >> $OUTPUT_CSV
    
    rm temp_execution_time.txt
done

