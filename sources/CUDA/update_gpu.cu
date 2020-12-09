#include <math.h>
#include "lcutil.h"
#include "timestamp.h"

// Kernel function with threads - one grid cell per thread
__global__ void kupdate(float* next, const float* prev, const int dim)
{
	// parameters for heat transfer
	struct Parms
	{ 
		float cx;
		float cy;
	} parms = {(float) 0.1, (float) 0.1};
	
	// Determine exact thread ID
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	
	// y -> row, x -> column, offset -> exact grid cell
	int y = index / (dim + 2);
	int x = index % (dim + 2);
	int offset = y * (dim + 2) + x;
	
	// Neighbours
	int left = offset - 1;
	int right = offset + 1;
	int top = offset - (dim + 2);
	int bottom = offset + (dim + 2);
	
	// make sure that thread's cell is not out of grid bounds & is inside the perimeter
	if (index < (dim + 2) * (dim + 2))
		if (y >= 1 && x >= 1 && y <= dim && x <= dim)
			next[offset] = prev[offset]
							+ parms.cx
								* (prev[left] + prev[right] - (float) 2.0 * prev[offset])
							+ parms.cy
								* (prev[top] + prev[bottom] - (float) 2.0 * prev[offset]);
}

extern "C" float update_gpu(float* next, const float* prev, const int dim, const int steps)
{
	int block_size;														// Number of threads per block
	int num_blocks;														// Number of blocks
	float* d_prev;														// Device source grid
	float* d_next;														// Device destination grid
	float* temp_grid;													// Two grids - one for before and one for after and temp points to "before"
	float* next_grid;													// next points to "after"
	int cur_grid;														// IF 0 -> source is current temperature grid ELSE dest ...
	timestamp s_time;
	float t_time;
	
	// Allocate memory for d_prev & d_next in the device
	CUDA_SAFE_CALL(cudaMalloc((void**) &d_prev, sizeof(float) * ((dim + 2) * (dim + 2))));
	CUDA_SAFE_CALL(cudaMalloc((void**) &d_next, sizeof(float) * ((dim + 2) * (dim + 2))));
	
	// Copy grids from CPU to device
	CUDA_SAFE_CALL(cudaMemset(d_next, 0, sizeof(float) * ((dim + 2) * (dim + 2))));
	CUDA_SAFE_CALL(cudaMemcpy(d_prev, prev, sizeof(float) * ((dim + 2) * (dim + 2)), cudaMemcpyHostToDevice));
	
	// Create a block of threads (Threads per block - maximum 1024 threads per block for GTX 480)
	block_size = 512;
	dim3 threads(block_size);
	
	// Define number of blocks of block_size we need
	num_blocks = FRACTION_CEILING((float) (dim + 2) * (dim + 2), (float) block_size);
	dim3 blocks(num_blocks);
	
	cur_grid = 0;
	
	// define start time for update
	s_time = getTimestamp();
	
	for (int i = 0; i < steps; i++)
	{
		// set "before" and "after" grid
		temp_grid = (cur_grid == 0 ? d_prev : d_next);
		next_grid = (cur_grid != 0 ? d_prev : d_next);
		
		// update grid
		kupdate<<<blocks, threads>>>(next_grid, temp_grid, dim);
		
		// swap grids
		cur_grid = 1 - cur_grid;
	}
	
	// calculate elapsed time for update
	t_time = getElapsedtime(s_time);
	
	// determine errors & synchronize threads
	CUDA_SAFE_CALL(cudaGetLastError());
	CUDA_SAFE_CALL(cudaThreadSynchronize());
	
	// copy destination grid from device to cpu
	CUDA_SAFE_CALL(cudaMemcpy(next, next_grid, sizeof(float) * ((dim + 2) * (dim + 2)), cudaMemcpyDeviceToHost));
	
	// free device grids
	CUDA_SAFE_CALL(cudaFree(d_prev));
	CUDA_SAFE_CALL(cudaFree(d_next));
	
	return t_time;
}
