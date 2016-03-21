#ifndef _LOGIN_
#define _LOGIN_

#include <openssl/sha.h>
#include "helpers.h"

#define USERNAME_LENGTH 64

int send_login(int socket);
int open_connection(char * servIP, char * servPort);
int login(int socket, char * read_buffer, char * write_buffer); 

#endif
