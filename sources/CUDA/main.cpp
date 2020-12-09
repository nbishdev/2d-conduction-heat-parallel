#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <string.h>
#include "header.h"
#include "timestamp.h"

int main(int argc, char **argv)
{
	int BLOCKS;															// Total grid blocks
	int STEPS;															// Time steps
	int DIM;															// Dimensions of matrix
	float *fgrid, *lgrid;												// Two grids - one for before and one for after and temp points to "before" while next points to "after"
	float f_time_cpu, f_time_gpu;										// For calculating elapsed time on CPU and GPU
	size_t free_CUDA_memory, total_CUDA_memory;							// For obtaining info about GPU memory

	// arguments count
	if (argc != 3)
	{
		printf("Usage: heat <grid_blocks> <time_steps>\n");
		exit(EXIT_FAILURE);
	}
	
	// arguments check
	if (arg_check(argv, &BLOCKS, &STEPS, &DIM) < 0)
	{
		printf("Arguments must be positive integers with first argument being an integer multiple of the number of processes\n");
		exit(EXIT_FAILURE);
	}
	
	// obtain info about GPU memory
	cudaMemGetInfo(&free_CUDA_memory, &total_CUDA_memory);
	printf("GPU memory stats\n");
	printf("\tFree:\t%u\n", (unsigned int) free_CUDA_memory);
	printf("\tTotal:\t%u\n", (unsigned int) total_CUDA_memory);
	printf("\tUsage:\t%.2f\%\n\n", (float) (total_CUDA_memory - free_CUDA_memory) * 100.0 / (float) total_CUDA_memory);
	
	// grids create
	if ((fgrid = grid_create(DIM + 2)) == NULL)
	{
		perror("grid_create: ");
		exit(EXIT_FAILURE);
	}
	
	if ((lgrid = grid_create(DIM + 2)) == NULL)
	{
		perror("grid_create: ");
		exit(EXIT_FAILURE);
	}
	
	// initialize grid & initialize to zero the destination grid
	grid_init(fgrid, DIM + 2);
	memset(lgrid, 0 , (DIM + 2) * (DIM + 2) * sizeof(float));
	
	// start grid print (for debugging purposes)
	//grid_print(fgrid, DIM + 2, "initial.dat");
	
	/* --> Execution on GPU <-- */
	f_time_gpu = update_gpu(lgrid, fgrid, DIM, STEPS);
	
	grid_init(fgrid, DIM + 2);
	memset(lgrid, 0 , (DIM + 2) * (DIM + 2) * sizeof(float));
	
	/* --> Execution on CPU <-- */
	f_time_cpu = update_cpu(lgrid, fgrid, DIM, STEPS);
	
	printf("Execution time on GPU: %f seconds\n", (float) f_time_gpu / (float) 1000.0);
	printf("GPU Bandwith: %f Mbps\n\n", 2 * (DIM + 2) * (DIM + 2) * sizeof(float) / f_time_gpu * 1000.0f / (float) (1024*1024));
	printf("Execution time on CPU: %f seconds\n", (float) f_time_cpu / (float) 1000.0);
	printf("CPU Bandwith: %f Mbps\n", 2 * (DIM + 2) * (DIM + 2) * sizeof(float) / f_time_cpu * 1000.0f / (float) (1024*1024));
	
	// final grid print (for debugging purposes)
	//grid_print(lgrid, DIM + 2, "final.dat");
	
	// free grids
	grid_destr(fgrid);
	grid_destr(lgrid);
	
	exit(EXIT_SUCCESS);
}
