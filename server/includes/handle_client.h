#ifndef _HANDLE_CLIENT_
#define _HANDLE_CLIENT_

#include "helpers.h"

void handle_client(int sock, char * ip_address);
void send_to_client(int sock, char * message);
int read_line_from_pipe(char * owner, char * buffer, size_t buffSize);
void send_to_receivers_pipe(char * receiver, char * message);
int socket_was_closed(int socket);
int horrible_timer(int sec);

#endif
