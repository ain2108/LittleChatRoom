#ifndef _HELPERS_
#define _HELPERS_

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include "login.h"
#include "read_write.h"

#define READ_BUFFER 128
#define READ_BUFFER_SIZE 2048
#define WRITE_BUFFER_SIZE 2048

void die_verbosely(char * message);


#endif
