#include "handle_client.h"
#include "database_interactions.h"

void handle_client(int sock, char * ip_address){

  fprintf(stderr, "\n==============================================\n");
  
  // Authenticate the user
  if(!authenticate(sock, ip_address))
    return;

  fprintf(stderr, "Client passed authentication\n");
  // Notify the user of success
  char smallBuff[64];
  memset(smallBuff, 0, 64);
  sprintf(smallBuff, "OK\nWelcome user!\n");
  send(sock, smallBuff, strlen(smallBuff), 0);

  // Echo
  fprintf(stderr, "Echo service ready to start\n");
  int charsRead = 0;
  char * line = (char *) malloc(sizeof(char) * READ_BUFFER_SIZE);
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

