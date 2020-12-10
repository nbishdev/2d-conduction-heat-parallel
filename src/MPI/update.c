// parameters for heat transfer
static struct Parms
{
	float cx;
	float cy;
} parms = {0.1, 0.1};

// updates only the internal grid - not perimeter
void update_center(float* dest, const float* source, const int num)
{
	int i, j;
	
	for (i = 2; i < num; i++) 
		for (j = 2; j < num; j++) 
			dest[i * (num + 2) + j] = source[i * (num + 2) + j] +
				parms.cx * ((source[(i + 1) * (num + 2) + j]) +
					source[(i - 1) * (num + 2) + j] - 2.0 * source[i * (num + 2) + j]) +
				parms.cy * (source[i * (num + 2) + (j + 1)] +
					source[i * (num + 2) + (j - 1)] - 2.0 * source[i * (num + 2) + j]);
}

// updates only the perimeter of the grid
void update_borders(float* dest, const float* source, const int num)
{
	int i, j;
	
	// top border
	i = 1;
	for (j = 1; j < num + 1; j++) 
		dest[i * (num + 2) + j] = source[i * (num + 2) + j] +
			parms.cx * ((source[(i + 1) * (num + 2) + j]) +
				source[(i - 1) * (num + 2) + j] - 2.0 * source[i * (num + 2) + j]) +
			parms.cy * (source[i * (num + 2) + (j + 1)] +
				source[i * (num + 2) + (j - 1)] - 2.0 * source[i * (num + 2) + j]);
	
	// bottom border
	i = num;
	for (j = 1; j < num + 1; j++) 
		dest[i * (num + 2) + j] = source[i * (num + 2) + j] +
			parms.cx * ((source[(i + 1) * (num + 2) + j]) +
				source[(i - 1) * (num + 2) + j] - 2.0 * source[i * (num + 2) + j]) +
			parms.cy * (source[i * (num + 2) + (j + 1)] +
				source[i * (num + 2) + (j - 1)] - 2.0 * source[i * (num + 2) + j]);
	
	// left border
	j = 1;
	for (i = 2; i < num; i++) 
		dest[i * (num + 2) + j] = source[i * (num + 2) + j] +
			parms.cx * ((source[(i + 1) * (num + 2) + j]) +
				source[(i - 1) * (num + 2) + j] - 2.0 * source[i * (num + 2) + j]) +
			parms.cy * (source[i * (num + 2) + (j + 1)] +
				source[i * (num + 2) + (j - 1)] - 2.0 * source[i * (num + 2) + j]);
	
	// right border
	j = num;
	for (i = 2; i < num; i++) 
		dest[i * (num + 2) + j] = source[i * (num + 2) + j] +
			parms.cx * ((source[(i + 1) * (num + 2) + j]) +
				source[(i - 1) * (num + 2) + j] - 2.0 * source[i * (num + 2) + j]) +
			parms.cy * (source[i * (num + 2) + (j + 1)] +
				source[i * (num + 2) + (j - 1)] - 2.0 * source[i * (num + 2) + j]);
}
