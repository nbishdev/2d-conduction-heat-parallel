#ifndef _HEADER_H_
#define _HEADER_H_

int arg_check(char**, int*, int*, int*);
float* grid_create(int dim);
int grid_init(float* grid, const int dim);
void grid_destr(float* grid);
void grid_print(const float* grid, const int dim, const char* file);
float update_cpu(float* dest, float* source, const int dim, const int steps);
extern "C" float update_gpu(float* dest, const float* source, const int dim, const int steps);

#endif
