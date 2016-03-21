#ifndef _HELPERS_
#define _HELPERS_

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

#include <time.h>
#include <signal.h>     /* for signal() */

#define READ_BUFFER 128
#define READ_BUFFER_SIZE 2048
#define WRITE_BUFFER_SIZE 2048

#define PASSWORD_LIMIT 32
#define USERNAME_LIMIT 32
#define IP_LENGTH 16
#define N_USERS 16
#define PASS_USRN_LENGTH 128
#define MAX_PENDING 8

#define USERS_FILE_NAME "user_pass.txt"
#define DATABASE_NAME "UsersRecDB"
#define IP_BAN_DB_NAME "bandb"
#define MAX_IP_DB_REC_N 128
#define MAX_FAILS 3

typedef struct UsersDBRec{
  int user_id;
  char login[PASSWORD_LIMIT];
  char password[USERNAME_LIMIT];
  int logged_in;
  long last_login_time;
  long last_logout_time;
  char ip[IP_LENGTH];

} UsersDBRec;

typedef struct UsersDB{
  UsersDBRec records[N_USERS];
} UsersDB;


void die_verbosely(char * message);
int readLine(FILE * sockf, char * buffer, size_t buffSize);
void read_usersDB(UsersDB * db, FILE * stream);
int sreadLine(int socket, char * buffer, size_t buffSize);

#include "setup.h"
	  
	  
       

#endif
