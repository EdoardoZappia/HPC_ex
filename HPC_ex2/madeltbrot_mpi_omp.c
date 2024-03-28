#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

short int mandelbrot(double real, double imag, int max_iter) {
    double z_real = real;
    double z_imag = imag;
    for (int n = 0; n < max_iter; n++) {
        double r2 = z_real * z_real;
        double i2 = z_imag * z_imag;
        if (r2 + i2 > 4.0) return n;
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = r2 - i2 + real;
    }
    return max_iter;
}

int main(int argc, char *argv[]) {
    int width = 800, height = 600;
    double x_left = -2.0, x_right = 1.0, y_lower = -1.0, y_upper = 1.0;
    int max_iterations = 255; // Adjust this if needed, keeping short int's limit in mind
    int world_size, world_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc == 7) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        x_left = atof(argv[3]);
        y_lower = atof(argv[4]);
        x_right = atof(argv[5]);
        y_upper = atof(argv[6]);
    }

    int rows_per_process = height / world_size;
    int remainder_rows = height % world_size;
    int start_row = world_rank * rows_per_process;
    int end_row = start_row + rows_per_process;
    if (world_rank == world_size - 1) {
        end_row += remainder_rows; // Handle remainder rows
    }

    short int* part_buffer = (short int*)malloc(width * (end_row - start_row) * sizeof(short int));

    #pragma omp parallel for schedule(dynamic)
    for (int j = start_row; j < end_row; j++) {
        for (int i = 0; i < width; i++) {
            double x = x_left + i * (x_right - x_left) / width;
            double y = y_lower + j * (y_upper - y_lower) / height;
            int index = (j - start_row) * width + i;
            part_buffer[index] = mandelbrot(x, y, max_iterations);
        }
    }

    short int* image_buffer = NULL;
    if (world_rank == 0) {
        image_buffer = (short int*)malloc(width * height * sizeof(short int));
    }

    int* recvcounts = NULL;
    int* displs = NULL;

    if (world_rank == 0) {
        recvcounts = (int*)malloc(world_size * sizeof(int));
        displs = (int*)malloc(world_size * sizeof(int));
        for (int i = 0; i < world_size; i++) {
            recvcounts[i] = width * rows_per_process;
            displs[i] = i * width * rows_per_process;
        }
        recvcounts[world_size - 1] += width * remainder_rows;
    }

    MPI_Gatherv(part_buffer, width * (end_row - start_row), MPI_SHORT,
                image_buffer, recvcounts, displs, MPI_SHORT,
                0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        FILE *file = fopen("image.pgm", "w");
        fprintf(file, "P2\n%d %d\n%d\n", width, height, max_iterations);
        for (int i = 0; i < width * height; i++) {
            fprintf(file, "%d ", image_buffer[i]);
            if ((i + 1) % width == 0) fprintf(file, "\n");
        }
        fclose(file);
        free(image_buffer);
        free(recvcounts);
        free(displs);
        printf("Mandelbrot set generated and saved to 'image.pgm'\n");
    }

    free(part_buffer);
    MPI_Finalize();
    return 0;
}

