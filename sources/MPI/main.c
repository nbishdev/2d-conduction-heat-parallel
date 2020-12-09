#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "header.h"

int main(int argc, char **argv)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double start_time, end_time;										// Master process time counters
	int myrank;															// Rank of process
	int numprocs;														// Number of processes
	int N;																// Grid blocks per process
	int BLOCKS;															// Total grid blocks
	int STEPS;															// Time steps
	int CHECK;															// Period of stability check
	int DIM;															// Block cells
	int mpi_dim[2];														// MPI grid topology dimension
	int period[2];														// MPI grid topology cyclic indicator
	int reorder;														// MPI grid reorder indicator
	int left, right, top, bottom;										// Appropriate neighbors
	int cur_grid;														// IF 0 -> fgrid is current temperature grid ELSE lgrid ...
	int counter;														// Step counter
	int namelen;														// Length of host name
	float *fgrid, *lgrid, *temp_grid, *next_grid;						// Two grids - one for before and one for after and temp points to "before" while next points to "after"
	MPI_Comm cartesian;														// New communicator
	MPI_Request send_req[4], recv_req[4];								// MPI requests for wait
	MPI_Status send_stat[4], recv_stat[4];								// MPI statuses for wait
	MPI_Datatype row;													// MPI vector datatype for rows
	MPI_Datatype column;												// MPI vector datatype for columns
	
	// initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	// arguments count
	if (argc != 3)
	{
		printf("Usage: heat <grid_blocks> <time_steps>\n");
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
	
	// arguments check
	if (arg_check(argv, numprocs, &N, &BLOCKS, &STEPS, &DIM) < 0)
	{
		printf("Arguments must be positive integers with first argument being an integer multiple of the number of processes\n");
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
	
	// topology parameters initialization
	mpi_dim[0] = sqrt(numprocs);
	mpi_dim[1] = mpi_dim[0];
	period[0] = 1;
	period[1] = 1;
	reorder = 1;
	
	// 2D topology create
	MPI_Cart_create(MPI_COMM_WORLD, 2, mpi_dim, period, reorder, &cartesian);
	
	// Topology neighbors
	MPI_Cart_shift(cartesian, 0, 1, &left, &right);
	MPI_Cart_shift(cartesian, 1, 1, &top, &bottom);
	
	// datatypes initialize
	MPI_Type_vector(DIM, 1, DIM + 2, MPI_FLOAT, &column);
	MPI_Type_commit(&column);
	MPI_Type_vector(DIM, 1, 1, MPI_FLOAT, &row);
	MPI_Type_commit(&row);
	
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
	
	// start grid initialize
	grid_init(fgrid, DIM + 2, myrank);
	
	// start grid print (for debugging purposes)
	//if (myrank == 0)
	//	grid_print(fgrid, DIM + 2, "initial.dat");
	
	cur_grid = 0;
	
	// set "before" and "after" grid
	temp_grid = (cur_grid == 0 ? fgrid : lgrid);
	next_grid = (cur_grid != 0 ? fgrid : lgrid);
	
	// wait for all hosts to be initialized - synchronize
	MPI_Barrier(cartesian);
	
	// define start time for update
	if (myrank == 0)
		start_time = MPI_Wtime();
	
	for (counter = 0; counter < STEPS; counter++)
	{
		if (numprocs == 1)
		{
			// update center --> place in other grid
			update_center(next_grid, temp_grid, DIM);
			
			// update perimeter --> place in other grid
			update_borders(next_grid, temp_grid, DIM);
		}
		else
		{
			// sends left
			MPI_Isend(&temp_grid[1 * (DIM + 2) + 1], 1, column, left, myrank, cartesian, &send_req[0]);
			
			// sends right
			MPI_Isend(&temp_grid[1 * (DIM + 2) + DIM], 1, column, right, myrank, cartesian, &send_req[1]);
			
			// sends top
			MPI_Isend(&temp_grid[1 * (DIM + 2) + 1], 1, row, top, myrank, cartesian, &send_req[2]);
			
			// sends bottom
			MPI_Isend(&temp_grid[DIM * (DIM + 2) + 1], 1, row, bottom, myrank, cartesian, &send_req[3]);
			
			
			// receives left
			MPI_Irecv(&temp_grid[1 * (DIM + 2) + 0], 1, column, left, left, cartesian, &recv_req[0]);
			
			// receives right
			MPI_Irecv(&temp_grid[1 * (DIM + 2) + (DIM + 1)], 1, column, right, right, cartesian, &recv_req[1]);
			
			// receives top
			MPI_Irecv(&temp_grid[0 * (DIM + 2) + 1], 1, row, top, top, cartesian, &recv_req[2]);
			
			// receives bottom
			MPI_Irecv(&temp_grid[(DIM + 1) * (DIM + 2) + 1], 1, row, bottom, bottom, cartesian, &recv_req[3]);
			
			
			// update center --> place in other grid
			update_center(next_grid, temp_grid, DIM);
			
			// wait for incoming requests
			MPI_Waitall(4, recv_req, recv_stat);
			
			// update boarders --> place in other grid
			update_borders(next_grid, temp_grid, DIM);
		}
		
		// wait for outgoing requests
		if (numprocs != 1)
			MPI_Waitall(4, send_req, send_stat);
		
		if (counter != STEPS - 1)
		{	
			// swap grids
			cur_grid = 1 - cur_grid;
		
			temp_grid = (cur_grid == 0 ? fgrid : lgrid);
			next_grid = (cur_grid != 0 ? fgrid : lgrid);
		}
	}
	
	// calculate elapsed time for update & print updated data
	if (myrank == 0)
	{
		end_time = MPI_Wtime();
		
		MPI_Get_processor_name(processor_name, &namelen);
		printf("Process %d running on %s finished in %f seconds\n", myrank, processor_name, end_time - start_time);
		
		// final grid print (for debugging purposes)
		//grid_print(next_grid, DIM + 2, "final.dat");
	}
	
	// free grids
	grid_destr(fgrid, DIM + 2);
	grid_destr(lgrid, DIM + 2);
	
	// free datatypes
	MPI_Type_free(&row);
	MPI_Type_free(&column);
	
	MPI_Finalize();

	exit(EXIT_SUCCESS);
}
