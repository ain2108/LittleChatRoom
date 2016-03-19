#ifndef _LOGIN_
#define _LOGIN_

#include <openssl/sha.h>
#include "helpers.h"

#define USERNAME_LENGTH 64

int send_login(FILE * socket);
int open_connection(char * servIP, char * servPort);

#endif
