#include "handle_client.h"

void handle_client(int sock){

  char * line = (char *) malloc(sizeof(char) * READ_BUFFER_SIZE);
  memset(line, 0, READ_BUFFER_SIZE);
  
  fprintf(stderr, "Somebody is here:)\n");


  // Login
  sreadLine(sock, line, READ_BUFFER_SIZE - 1);
  fprintf(stderr, "Client sent: %s\n", line);
  send(sock, "OK\n", 3, 0);
  send(sock, "Welcome user!\n", 20, 0);

  fprintf(stderr, "Stuff was written, but client is not reading.\n");


  // Echo
  int charsRead = 0;
  memset(line, 0, READ_BUFFER_SIZE);
  while((charsRead = sreadLine(sock, line, READ_BUFFER_SIZE - 1)) > 0){
    fprintf(stderr, "Client sent: %s\n", line);
    send(sock, line, strlen(line), 0);
    send(sock, "\n", 1, 0);
    memset(line, 0, READ_BUFFER_SIZE);
  }

  
  fprintf(stderr, "Closing connection\n");
  close(sock);
  return;
}
