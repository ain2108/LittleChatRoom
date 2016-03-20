#include "handle_client.h"

void handle_client(int sock){

  char line[READ_BUFFER_SIZE];
  memset(line, 0, READ_BUFFER_SIZE);
  
  fprintf(stderr, "Somebody is here:)\n");


  // Login
  recv(sock, line, READ_BUFFER_SIZE - 1, 0);
  fprintf(stderr, "Client sent: %s\n", line);
  send(sock, "OK\n", 3, 0);
  send(sock, "Welcome user!\n", 20, 0);

  fprintf(stderr, "Stuff was written, but client is not reading.\n");


  // Echo
  int charsRead = 0;
  memset(line, 0, READ_BUFFER_SIZE);
  while( (charsRead = recv(sock, line, READ_BUFFER_SIZE - 1, 0)) > 0){
    send(sock, line, READ_BUFFER_SIZE, 0);
    memset(line, 0, READ_BUFFER_SIZE);
  }

  close(sock);
  return;
}
