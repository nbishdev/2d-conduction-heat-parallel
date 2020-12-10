#include <ctype.h>
#include <stdlib.h>
#include <math.h>

int isnumber(char*);
int isnotzero(char*);

int arg_check(char** argv, const int numprocs, int* n, int* total, int* counter, int* dim)
{	
	// Grid blocks & steps must be positive integers
	if (!isnotzero(argv[1]) || !isnotzero(argv[2]))
		return -1;
	
	*total = atoi(argv[1]);
	*counter = atoi(argv[2]);
	
	// Number of processes must have a square root
	if (sqrt(numprocs) != (int) sqrt(numprocs))
		return -1;
	
	// Grid must be a square matrix
	if (sqrt(*total) != (int) sqrt(*total))
		return -1;
	
	// Grid / processes must be an integer
	if ((*total % numprocs) != 0)
		return -1;
	
	// Local grid (per process) must be a square matrix
	if (sqrt(*total / numprocs) != (int) sqrt(*total / numprocs))
		return -1;
	
	*n = sqrt(numprocs);
	*dim = sqrt(*total / numprocs);
	
	return 1;
}

int isnotzero(char* string)
{
	int flag;
	
	if (isnumber(string))
	{
		flag = 0;
		while (*string != '\0')
		{
			if (*string != '0')
				flag = 1;
			string++;
		}
		return flag;
	}
	
	return 0;
}

int isnumber(char* string)
{
	while (*string != '\0')
	{
		if (!isdigit(*string))
			return 0;
		string++;
	}
	return 1;
}
