#include <stdio.h>
#include <stdlib.h>

float* grid_create(const int num)
{
	float* grid;
	
	grid = malloc(num * num * sizeof(float));
	
	return grid;
}

void grid_init(float* grid, const int num)
{
	int i, j;
	
	for (i = 0; i < num; i++)
		for (j = 0; j < num; j++)
		{
			if (i == 0 || i == num - 1 || j == 0 || j == num - 1)
				grid[i * num + j] = 0;
			else
				grid[i * num + j] = (float) ((i - 1) * (num - (i + 1) - 1) * (j - 1) * (num - (j + 1) - 1));
		}
}

void grid_destr(float* grid)
{
	free(grid);
}

void grid_print(const float* grid, const int num, const char* file)
{
	int i, j;
	FILE *fp;
	
	fp = fopen(file, "w");
	for (i = 1; i < num - 1; i++)
	{
		for (j = 1; j < num - 1; j++)
			fprintf(fp, "\t%6.1f\t", grid[i * num + j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}
