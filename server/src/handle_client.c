#include "handle_client.h"

void handle_client(int sock){

  // Creating FILE pointers for convenience
  FILE * sockf_write = fdopen(sock, "w");
  FILE * sockf_read = fdopen(sock, "r");
  char line[2048];
  memset(line, 0, 2048);
  
  // Login
  readLine(sockf_read, line, 2048 - 1);
  fprintf(sockf_write, "OK\n");
  fprintf(sockf_write, "Welcome user!\n");

  // Echo
  int charsRead = 0;
  while( (charsRead = readLine(sockf_read, line, 2048 - 1)) >= 0){
    fprintf(sockf_write, "%s\n", line);
  }


  close(sock);
  return;
}
