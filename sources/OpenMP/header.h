#ifndef _HEADER_H_
#define _HEADER_H_

int arg_check(char**, const int, int*, int*, int*, int*);
float* grid_create(const int);
void grid_init(float*, const int, const int);
void grid_destr(float*, const int);
void update_center(float* dest, const float* source, const int num);
void update_borders(float* dest, const float* source, const int num);

#endif
