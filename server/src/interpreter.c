#include "interpreter.h"
#include "handle_client.h"
#include "database_interactions.h"

#define I_HATE_STRINGS 66

int interpret(int socket, char * read_line, UsersDBRec * users_rec){

  // Load UsersDB
  UsersDB users_db;
  FILE * users_db_file = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file, &users_db);
  fclose(users_db_file);  

  // Get the comman from the line
  char * token;
  read_line[strlen(read_line)] = ' ';
  token = strtok(read_line, " ");
  
  // 'switch' on the token
  if(!strcmp(token, COMMAND_WHO)){
    perform_WHO(socket, &users_db);
    return 0;

  }else if(!strcmp(token, COMMAND_LAST)){
    token = strtok(NULL, "\0");
    if(token == NULL) return 0;
    int min;
    if((min = atoi(token)) == 0){
      send_to_client(socket, "server: bad format\n");
      return 0;
    }
    perform_LAST(socket, min, &users_db);
    return 0;
  }else if(!strcmp(token, COMMAND_BROADCAST)){
    token = strtok(NULL, "\0");
    perform_BROADCAST(token, users_rec, &users_db);
    return 0;
  }else if(!strcmp(token, COMMAND_SEND)){
    char * receiver_msg = strtok(NULL, "\0");
    // If receiver was not provided
    if(receiver_msg == NULL){
      send_to_client(socket, "server: ");
      send_to_client(socket, "please provide destination\n");   
      return 0;
    }
    // If its a multicast message
    if( ((*receiver_msg) == '(') && strstr(receiver_msg, ")") != NULL){
      char temp[READ_BUFFER_SIZE];
      memset(temp, 0, READ_BUFFER_SIZE);
      strcpy(temp, receiver_msg);
      perform_MULTI_SEND(temp, &users_db, socket, users_rec);
      return 0;
    }
    perform_SEND(receiver_msg, &users_db, socket, users_rec);
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
  // Buffer
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

void perform_LAST(int socket, int min, UsersDB * db){
  // Buffer
  char write_line[WRITE_BUFFER_SIZE];
  memset(write_line, 0, WRITE_BUFFER_SIZE);
  int seconds = min * 60; // 60 seconds in minute
  int i;
  strcpy(write_line, "server: "); 

  // Current time
  time_t time_sec;
  time(&time_sec);

  // Iterate over the database
  for(i = 0; i < N_USERS; i++){
    // The case of first login by the user
    if(db->records[i].logged_in == 1){
      strcat(write_line, db->records[i].login);
      strcat(write_line, " ");
      continue;
    }
    if((time_sec - db->records[i].last_logout_time) < seconds){
      strcat(write_line, db->records[i].login);
      strcat(write_line, " ");
    }
  }
  
  strcat(write_line, "\n");
  send_to_client(socket, write_line);
  return;

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

void perform_SEND(char * receiver_message, UsersDB * db, 
		  int socket, UsersDBRec * rec){
  receiver_message[strlen(receiver_message)] = ' ';
  char * token = strtok(receiver_message, " ");
  if(token == NULL){
    send_to_client(socket, "server: ");
    send_to_client(socket, "please provide destination\n");   
    return;
  }
  if(!check_username_exits_and_online(token, db)){
    send_to_client(socket, "server: ");
    send_to_client(socket, token);
    send_to_client(socket, " offline or does not exits\n");
    return;
  }
  
  // Extract username
  char username[USERNAME_LIMIT];
  memset(username, '\0', USERNAME_LIMIT);
  strcpy(username, token);
  token = strtok(NULL, "\0");
  if( (*token) == '\0') return;

  send_to_receivers_pipe(username, rec->login);
  send_to_receivers_pipe(username, ": ");
  send_to_receivers_pipe(username, token);
  send_to_receivers_pipe(username, "\n");

  return;
}

int check_username_exits_and_online(char * login, UsersDB * db){

  // Iterate through database, check stuff.
  int i;
  for(i = 0; i < N_USERS; i++){
    if( !strcmp(db->records[i].login, login) && db->records[i].logged_in){
      return 1;
    }
  }
  return 0; // Either does not exist or offline
}

void perform_MULTI_SEND(char * recs_msg, UsersDB * db, 
			int socket, UsersDBRec * rec){

  char * open = strstr(recs_msg, "(");
  char * closed = strstr(recs_msg, ")");

  // Get the message
  char message[WRITE_BUFFER_SIZE];
  memset(message, 0, WRITE_BUFFER_SIZE);
  strcpy(message, closed + 1);

  // Get the receivers
  char receivers[WRITE_BUFFER_SIZE];
  memset(receivers, 0, WRITE_BUFFER_SIZE);
  strncpy(receivers, open + 1, closed - open - 1);

  // Tokenize the string and get the pointers
  char *damn_tokens[I_HATE_STRINGS];
  memset(damn_tokens, 0, I_HATE_STRINGS);
  char * the_token = strtok(receivers, " ");
  int i = 0;
  while(the_token != NULL){
    damn_tokens[i] = the_token;
    the_token = strtok(NULL, " ");
    i++;
  }
  
  // Buffer to construct the message for unary send
  char buffer[WRITE_BUFFER_SIZE];
  memset(buffer, 0, WRITE_BUFFER_SIZE);
  
  int j = 0; 
  while(j < i){
    the_token = damn_tokens[j];
    
    // Construct the proper message
    strcpy(buffer, the_token);
    strcat(buffer, " ");
    strcat(buffer, message);
    strcat(buffer, "\n");
    // Perform unary send
    
    perform_SEND(buffer, db, socket, rec);
    // Clean up and increment to walk through pointers
    memset(buffer, 0, WRITE_BUFFER_SIZE);
    j++;
  }
  return;
} 






