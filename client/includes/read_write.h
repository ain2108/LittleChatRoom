#ifndef _READ_WRITE_
#define _READ_WRITE_

#include "helpers.h"

void f_continous_reading(FILE * from, FILE * to);
void s_continous_reading(int from, int to);
int readLine(FILE * sockf, char * buffer, size_t buffSize);

#endif

