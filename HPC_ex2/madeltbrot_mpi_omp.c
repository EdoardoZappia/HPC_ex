#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <sys/time.h>

short int mandelbrot(double real, double imag, int max_iter) {
    double z_real = real;
    double z_imag = imag;
    for (int n = 0; n < max_iter; n++) {
        double r2 = z_real * z_real;
        double i2 = z_imag * z_imag;
        if (r2 + i2 > 4.0) return n;  // check the condition on the module, returns the number of iterations
                                      // if the point doesn't belong to the Mandelbrot set
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = r2 - i2 + real;
    }
    return max_iter;  // returns max_iter if the point belongs to the Mandelbrot set
}

// function to measure time in microseconds
long long current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

void save_image(short int* image_buffer, int width, int height, int max_iterations) {
    FILE *file = fopen("mandelbrot.pgm", "w");
    fprintf(file, "P2\n%d %d\n%d\n", width, height, max_iterations);
    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%d ", image_buffer[i]);
        if ((i + 1) % width == 0) fprintf(file, "\n");
    }
    fclose(file);
    printf("Mandelbrot set generated and saved to 'mandelbrot.pgm'\n");
}

int main(int argc, char *argv[]) {

    // default values
    int width = 800, height = 600;
    double x_left = -2.0, x_right = 1.0, y_lower = -1.0, y_upper = 1.0;
    int max_iterations = 255;

    long long start_time, end_time; // variables to measure execution time

    int world_size, world_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc != 8) {
        if (world_rank == 0) {
            printf("Usage: %s width height x_left x_right y_lower y_upper max_iterations\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    x_left = atof(argv[3]);
    y_lower = atof(argv[4]);
    x_right = atof(argv[5]);
    y_upper = atof(argv[6]);
    max_iterations = atoi(argv[7]);

    // weak scaling analysis
    if (world_rank == 0) {
        printf("Weak scaling analysis:\n");
        printf("Processors\tTime (ms)\tSpeedup\t\tEfficiency\n");
    }

    for (int num_procs = 1; num_procs <= world_size; num_procs++) {
        int rows_per_process = height / num_procs;
        int remainder_rows = height % num_procs;
        int start_row = world_rank * rows_per_process;
        int end_row = start_row + rows_per_process;
        if (world_rank == world_size - 1) {
            end_row += remainder_rows;
        }

        // allocates memory for partial results of each process
        short int* part_buffer = (short int*)malloc(width * (end_row - start_row) * sizeof(short int));

        // start measuring execution time
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = current_time();

        // OpenMP parallelization
    // parallelization on a lot of cores. Each thread will calculate a part of the image
        #pragma omp parallel for schedule(dynamic)  // dynamic scheduling: load distributed among the threads without an order
        for (int j = start_row; j < end_row; j++) {
            for (int i = 0; i < width; i++) {
                double x = x_left + i * (x_right - x_left) / width;
                double y = y_lower + j * (y_upper - y_lower) / height;
                int index = (j - start_row) * width + i;
                part_buffer[index] = mandelbrot(x, y, max_iterations);
            }
        }

        // End measuring execution time
        MPI_Barrier(MPI_COMM_WORLD);
        end_time = current_time();

        // Gather execution times from all processes
        long long total_time;
        MPI_Reduce(&end_time, &total_time, 1, MPI_LONG_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

        if (world_rank == 0) {
            double execution_time = (double)(total_time - start_time) / 1000.0; // Convert to milliseconds
            double speedup = ((double)world_size * (double)start_time) / (double)total_time;
            double efficiency = speedup / (double)world_size;

            printf("%d\t\t%.2f\t\t%.2f\t\t%.2f%%\n", num_procs, execution_time, speedup, efficiency * 100);
        }
        free(part_buffer);
    }

    // perform strong scaling analysis
    if (world_rank == 0) {
        printf("\nStrong scaling analysis:\n");
        printf("Processors\tTime (ms)\tSpeedup\t\tEfficiency\n");
    }

    for (int num_procs = 1; num_procs <= world_size; num_procs++) {
        if (world_rank < num_procs) {
            int rows_per_process = height / num_procs;
            int remainder_rows = height % num_procs;
            int start_row = world_rank * rows_per_process;
            int end_row = start_row + rows_per_process;
            if (world_rank == num_procs - 1) {
                end_row += remainder_rows;
            }

            short int* part_buffer = (short int*)malloc(width * (end_row - start_row) * sizeof(short int));

    // start measuring execution time
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = current_time();

        #pragma omp parallel for schedule(dynamic)
        for (int j = start_row; j < end_row; j++) {
            for (int i = 0; i < width; i++) {
                double x = x_left + i * (x_right - x_left) / width;
                double y = y_lower + j * (y_upper - y_lower) / height;
                int index = (j - start_row) * width + i;
                part_buffer[index] = mandelbrot(x, y, max_iterations);
            }
        }    

        // End measuring execution time
        MPI_Barrier(MPI_COMM_WORLD);
        end_time = current_time();

        // Gather execution times from all processes
        long long total_time;
        MPI_Reduce(&end_time, &total_time, 1, MPI_LONG_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

        if (world_rank == 0) {
            double execution_time = (double)(total_time - start_time) / 1000.0; // Convert to milliseconds
            double speedup = ((double)world_size * (double)start_time) / (double)total_time;
            double efficiency = speedup / (double)world_size;

            printf("%d\t\t%.2f\t\t%.2f\t\t%.2f%%\n", num_procs, execution_time, speedup, efficiency * 100);
        }

        free(part_buffer);
        }
    }
    MPI_Finalize();
    return 0;
}



