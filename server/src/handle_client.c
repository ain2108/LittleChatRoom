#include "handle_client.h"
#include "database_interactions.h"

void handle_client(int sock, char * ip_address){

  fprintf(stderr, "\n==============================================\n");
  
  // Authenticate the user
  int position;
  if((position = authenticate(sock, ip_address)) == -1)
    return;
  fprintf(stderr, "%s passed authentication\n", ip_address);

  // Notify the user of success
  send_to_client(sock, "OK\n");

  // Acquire position of username in UsersDB
  int offset = position * sizeof(UsersDBRec);
  UsersDBRec users_rec;
  read_UDBRec_from_file(&users_rec, offset);
  fillin_UsersDBRec(&users_rec, ip_address);
  write_UDBRec_from_file(&users_rec, offset);

  /*
  // Quickly check if everything is working as wanted
  FILE * users_db_file1 = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file1, &users_db);
  read_usersDB(&users_db, stderr);
  fclose(users_db_file1);  
  // Ready to deal with the service
  */
  
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

// Little function that sends a message to the client
void send_to_client(int sock, char * message){
  char buffer[WRITE_BUFFER_SIZE];
  memset(buffer, 0, WRITE_BUFFER_SIZE);
  sprintf(buffer, "%s", message);
  send(sock, buffer, strlen(buffer), 0);
  return;
}

