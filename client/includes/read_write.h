#ifndef _READ_WRITE_
#define _READ_WRITE_

#include "helpers.h"

int readLine(FILE * sockf, char * buffer, size_t buffSize);
int sreadLine(int socket, char * buffer, size_t buffSize);

#endif

