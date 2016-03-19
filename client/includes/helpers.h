#ifndef _HELPERS_
#define _HELPERS_

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define READ_BUFFER 128

void die_verbosely(char * message);
void f_continous_reading(FILE * from, FILE * to);
void s_continous_reading(int from, int to);

#endif
