#ifndef _LOGIN_
#define _LOGIN_

#include <openssl/sha.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USERNAME_LENGTH 64

int login(FILE * socket);

#endif
