import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def plot_scaling(filename, title):
    data = pd.read_csv('execution_times_mpi.csv', header=None)
    x = data[:, 0]
    y = data[:, 1]

    plt.plot(x, y, marker='o')
    plt.xlabel('Number of MPI Processes')
    plt.ylabel('Execution Time (seconds)')
    plt.title(title)
    plt.grid(True)
    plt.show()

    # Plot Strong Scaling
plot_scaling('execution_times_mpi.csv', 'Strong Scaling')

# Plot Weak Scaling
#plot_scaling('weak_scaling_data.txt', 'Weak Scaling')

