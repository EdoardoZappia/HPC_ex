#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

unsigned char mandelbrot(double real, double imag, int max_iter) {
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
    MPI_Init(&argc, &argv);

    double global_start_time = MPI_Wtime(); // Start timing here

    int width = 800, height = 600;
    double x_left = -2.0, x_right = 1.0, y_lower = -1.0, y_upper = 1.0;
    int max_iterations = 255;
    int world_size, world_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Parse arguments
    if (argc == 7) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        x_left = atof(argv[3]);
        y_lower = atof(argv[4]);
        x_right = atof(argv[5]);
        y_upper = atof(argv[6]);
    }

    // Distribute rows among processes
    int rows_per_process = height / world_size;
    int remainder_rows = height % world_size;
    int start_row = world_rank * rows_per_process;
    int end_row = start_row + rows_per_process + (world_rank == world_size - 1 ? remainder_rows : 0);

    unsigned char* part_buffer = (unsigned char*)malloc(width * (end_row - start_row) * sizeof(unsigned char));

    #pragma omp parallel for schedule(dynamic)
    for (int j = start_row; j < end_row; j++) {
        for (int i = 0; i < width; i++) {
            double x = x_left + i * (x_right - x_left) / width;
            double y = y_lower + j * (y_upper - y_lower) / height;
            int index = (j - start_row) * width + i;
            part_buffer[index] = mandelbrot(x, y, max_iterations);
        }
    }

    // Gather parts of the Mandelbrot set computed by each process
    unsigned char* image_buffer = NULL;
    if (world_rank == 0) {
        image_buffer = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    }

    MPI_Gather(part_buffer, width * (end_row - start_row), MPI_UNSIGNED_CHAR, image_buffer, width * (end_row - start_row), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        FILE *file = fopen("image.pgm", "w");
        fprintf(file, "P5\n%d %d\n255\n", width, height);
        fwrite(image_buffer, sizeof(unsigned char), width * height, file);
        fclose(file);
        printf("Mandelbrot set generated and saved to 'image.pgm'\n");
        free(image_buffer);
    }

    free(part_buffer);

    double global_end_time = MPI_Wtime(); // End timing here
	
    // Alla fine del main, prima di MPI_Finalize
if (world_rank == 0) {
    FILE *temp_file = fopen("temp_execution_time.txt", "w");
    if (temp_file != NULL) {
        fprintf(temp_file, "%f", global_end_time - global_start_time);
        fclose(temp_file);
    } else {
        printf("ERROR OPENING FILE\n");
    }
}
    MPI_Finalize();
    return 0;
}
