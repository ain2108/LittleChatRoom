#ifndef _SETUP_
#define _SETUP_

#include "helpers.h"

int setup_server_socket(unsigned short port);
void initialize_database(char * source_file_name, char * db_name, UsersDB * db);

#endif
