#include "interpreter.h"
#include "handle_client.h"
#include "database_interactions.h"

void interpret(int socket, char * read_line, UsersDBRec * users_rec){

  // Load UsersDB
  UsersDB users_db;
  FILE * users_db_file = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file, &users_db);
  fclose(users_db_file);  

  // Get the comman from the line
  char * command;
  command = strtok(read_line, " ");

  // 'switch' on the token
  if(!strcmp(command, COMMAND_WHO)){
    perform_WHO(socket, &users_db);
    return;

  }else if(!strcmp(command, COMMAND_LAST)){
  
  }else if(!strcmp(command, COMMAND_BROADCAST)){
  
  }else if(!strcmp(command, COMMAND_SEND)){
  
  }else if(!strcmp(command, COMMAND_LOGOUT)){
  
  }else{
    // DEFAULT CASE
    send_to_client(socket, "server: no such command\n");
    return;
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

void perform_BROADCAST(){

}

void perform_SEND(){

}

void perform_LOGOUT(){

}


