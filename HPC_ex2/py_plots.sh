#!/bin/bash
#SBATCH --job-name=PlottingJob
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=01:00:00
#SBATCH --partition=THIN

module load python  # Load Python module
module load matplotlib  # Load Matplotlib module (if needed)

python plots.py  # Run the Python script

