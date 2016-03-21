#ifndef _HANDLE_CLIENT_
#define _HANDLE_CLIENT_

#include "helpers.h"

void handle_client(int sock, char * ip_address);
void send_to_client(int sock, char * message);

#endif
