#include "handle_client.h"
#include "database_interactions.h"
#include "interpreter.h"

void handle_client(int sock, char * ip_address){
  
  // Authenticate the user
  int position;
  if((position = authenticate(sock, ip_address)) == -1){
    return;
  }

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
  UsersDB users_db;
  FILE * users_db_file1 = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file1, &users_db);
  read_usersDB(&users_db, stderr);
  fclose(users_db_file1);  
  // Ready to deal with the service
  */

  // We need a process that continously reads from owner's pipe.
  pid_t pid_reader;
  // Die in case fork fails
  if((pid_reader = fork()) < 0){
    die_verbosely("fork() failed");  // Should not die necessarily. 
  }

  /************************ READER PROCESS ****************************/
  if(pid_reader == 0){
    char read_line[READ_BUFFER_SIZE];
    memset(read_line, 0, READ_BUFFER_SIZE);

    // Read from the pipe as long as the connection with client open
    while(1){
      // Read from your own pipe
      read_line_from_pipe(users_rec.login, read_line, READ_BUFFER_SIZE);
      // Dump the read line into the socket
      // send_to_client(sock, pipe_line);
      // send_to_client(sock, "\n");
      interpret(sock, read_line, &users_rec); 
      memset(read_line, 0, READ_BUFFER_SIZE);
    }

    fprintf(stderr, "%s's reader process exits\n", users_rec.login);
    // close(sock);
    exit(0);
  }
  /*********************** READER PROCESS ENDS*************************/


  /********************* CONTROLLER PROCCESS **************************/
  fprintf(stderr, "serving %s\n", users_rec.login);
  int charsRead = 0;
  char line[READ_BUFFER_SIZE];
  memset(line, 0, READ_BUFFER_SIZE);
  while((charsRead = sreadLine(sock, line, READ_BUFFER_SIZE - 1)) > 0){
    fprintf(stderr, "%s sent: %s\n", users_rec.login, line);
    send_to_receivers_pipe(users_rec.login, line);
    send_to_receivers_pipe(users_rec.login, "\n");
    memset(line, 0, READ_BUFFER_SIZE);
  }

  fprintf(stderr, "closing connection with %s\n", ip_address);
  kill(pid_reader, SIGKILL); // kill the child
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

// Sends the message to the receivers pipe
void send_to_receivers_pipe(char * receiver, char * message){ 
  
  // Get hold of the pipe name
  char pipe_name[PIPE_NAME_LIMIT];
  memset(pipe_name, 0, PIPE_NAME_LIMIT);
  strcpy(pipe_name, PIPE_DIRECTORY);
  strcat(pipe_name, receiver);
  strcat(pipe_name, PIPE_NAME_POSTFIX);

  // Open the pipe
  int fd = open(pipe_name, O_WRONLY);
  write(fd, message, strlen(message));

  // Close the pip
  close(fd);
}

// Read a line from own pipe.
int read_line_from_pipe(char * owner, char * buffer, size_t buffSize){
  
  // Just to make sure
  memset(buffer, 0, buffSize);

  // Get hold of the pipe name                                               
  char pipe_name[PIPE_NAME_LIMIT];
  memset(pipe_name, 0, PIPE_NAME_LIMIT);
  strcpy(pipe_name, PIPE_DIRECTORY);
  strcat(pipe_name, owner);
  strcat(pipe_name, PIPE_NAME_POSTFIX);

  // Open the pipe
  int fd = open(pipe_name, O_RDONLY);

  // Read character by character looking for '\n'
  int bytesRead = 0;
  int bytERead = 0;
  while(bytesRead < buffSize){
    
    if( (bytERead = read(fd, buffer, 1)) == 0) return 0; // EOF
    else if(bytERead == -1) return -1; // Error
    
    if( (*buffer) == '\n'){
      *buffer = '\0';
      close(fd);
      return bytesRead + 1;  
    }
    buffer++;
    bytesRead++;
  }
  close(fd);
  return -2; // To indicate that the buffer was too large
}

// Check if the socket is closed
int socket_was_closed(int socket){
  char buffer[READ_BUFFER_SIZE];
  memset(buffer, 0, READ_BUFFER_SIZE);
  int status;
  if((status = recv(socket, buffer, READ_BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT)) <= 0){
    return 1;
  }
  return 0;
}

