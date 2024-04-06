#!/bin/bash
#SBATCH --job-name=HPC-MPI-Scaling
#SBATCH --time=02:00:00
#SBATCH --partition=THIN
#SBATCH --exclusive

module load openMPI/4.1.5/gnu/12.2.1

export OMP_NUM_THREADS=1

# Definiamo il numero di nodi per l'analisi di weak scaling
weak_nodes=2

# Definiamo il numero di task per nodo
tasks_per_node=24

# Definiamo la dimensione base del problema per l'analisi di strong scaling
strong_base_size=800

# Definiamo la dimensione base del problema per l'analisi di weak scaling
weak_base_size=400

# Calcoliamo il numero totale di task MPI per l'analisi di strong scaling
strong_total_tasks=$((SLURM_JOB_NUM_NODES * tasks_per_node))

# Strong Scaling: manteniamo la dimensione del problema costante
echo "Strong Scaling Analysis"
echo "Running with $strong_total_tasks MPI tasks and problem size $strong_base_size."
strong_output=$(mpirun -np $strong_total_tasks ./mandelbrot $strong_base_size $strong_base_size -2.0 -1.0 1.0 1.0 255)
echo "$strong_output"
echo "Strong Scaling analysis completed."

# Weak Scaling: aumentiamo la dimensione del problema proporzionalmente al numero di nodi
echo "Weak Scaling Analysis"
for ((weak_nodes=1; weak_nodes <= SLURM_JOB_NUM_NODES; weak_nodes++))
do
    weak_size=$((weak_base_size * weak_nodes))
    weak_total_tasks=$((weak_nodes * tasks_per_node))
    echo "Running with $weak_total_tasks MPI tasks and problem size $weak_size."
    weak_output=$(mpirun -np $weak_total_tasks ./mandelbrot $weak_size $weak_size -2.0 -1.0 1.0 1.0 255)
    echo "$weak_output"
    echo "Weak Scaling analysis for $weak_nodes nodes completed."
done

