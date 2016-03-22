#ifndef _SETUP_
#define _SETUP_

#include "helpers.h"

int setup_server_socket(unsigned short port);
void initialize_database(char * source_file_name, char * db_name, UsersDB * db);
void create_FIFOs(UsersDB * users_db);
void send_to_receivers_pipe(char * receiver, char * message);
#endif
