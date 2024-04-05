import numpy as np
import matplotlib.pyplot as plt

def plot_scaling(filename, title):
    data = np.loadtxt(filename)
    x = data[:, 0]
    y = data[:, 1]

    plt.plot(x, y, marker='o')
    plt.xlabel('Number of MPI Processes')
    plt.ylabel('Execution Time (seconds)')
    plt.title(title)
    plt.grid(True)
    plt.show()

# Plot Weak Scaling
plot_scaling('weak_scaling_data.txt', 'Weak Scaling')

# Plot Strong Scaling
plot_scaling('strong_scaling_data.txt', 'Strong Scaling')

