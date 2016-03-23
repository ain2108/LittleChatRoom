#include "interpreter.h"
#include "handle_client.h"
#include "database_interactions.h"

int interpret(int socket, char * read_line, UsersDBRec * users_rec){

  // Load UsersDB
  UsersDB users_db;
  FILE * users_db_file = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file, &users_db);
  fclose(users_db_file);  

  // Get the comman from the line
  char * token;
  token = strtok(read_line, " ");

  // 'switch' on the token
  if(!strcmp(token, COMMAND_WHO)){
    perform_WHO(socket, &users_db);
    return 0;

  }else if(!strcmp(token, COMMAND_LAST)){
    return 0;
  }else if(!strcmp(token, COMMAND_BROADCAST)){
    token = strtok(NULL, "\0");
    perform_BROADCAST(token, users_rec, &users_db);
    return 0;
  }else if(!strcmp(token, COMMAND_SEND)){
    return 0;
  }else if(!strcmp(token, COMMAND_LOGOUT)){
    send_to_client(socket, "server: logging out...\n");
    return 1;
  }else{
    // DEFAULT CASE
    send_to_client(socket, "server: no such command\n");
    return 0;
  }
}

void perform_WHO(int socket, UsersDB * db){
  char write_line[WRITE_BUFFER_SIZE];
  memset(write_line, 0, WRITE_BUFFER_SIZE);
  int i;
  strcpy(write_line, "server: "); 
  for(i = 0; i < N_USERS; i++){
    if(db->records[i].logged_in == 1){
      strcat(write_line, db->records[i].login);
      strcat(write_line, " ");
    }
  }
  strcat(write_line, "\n");
  send_to_client(socket, write_line);
  return;
}

void perform_LAST(){

}

void perform_BROADCAST(char * message, UsersDBRec * users_rec, UsersDB * db){
  
  // Write buffer
  char write_line[WRITE_BUFFER_SIZE];
  memset(write_line, 0, WRITE_BUFFER_SIZE);
 
  // Construct complete message 
  strcpy(write_line, users_rec->login);
  strcat(write_line, ": ");
  strcat(write_line, message);
  
  // Loop over users and put message into respective FIFOs
  int i;
  for(i = 0; i < N_USERS; i++){
    // To avoid sending the message to oneself
    if(!strcmp(db->records[i].login, users_rec->login)) continue;
    // Not to send to offline users
    if(db->records[i].logged_in == 0) continue;
    
    // Send data into the FIFO
    send_to_receivers_pipe(db->records[i].login, write_line);
    send_to_receivers_pipe(db->records[i].login, "\n");
  }
  return;
}

void perform_SEND(){

}






