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

  // Check if the user is already online
  int offset = position * sizeof(UsersDBRec);
  UsersDBRec users_rec;
  read_UDBRec_from_file(&users_rec, offset);
  if(users_rec.logged_in == 1){
    fprintf(stderr, "%s already online\n", users_rec.login);
    fprintf(stderr, "closing connection with %s\n", ip_address);
    close(sock);
    return;
  }

  // Notify the user of success
  send_to_client(sock, "OK\n");
  fillin_UsersDBRec(&users_rec, ip_address);
  write_UDBRec_from_file(&users_rec, offset);

  // We need a process that continously reads from owner's pipe.
  pid_t pid_reader;
  // Die in case fork fails
  if((pid_reader = fork()) < 0){
    die_verbosely("fork() failed");  // Should not die necessarily. 
  }

  /************************ READER PROCESS ****************************/
  if(pid_reader == 0){
    // Child asks the kernel to kill it if parent dies 
    prctl(PR_SET_PDEATHSIG, SIGKILL);

    char pipe_line[READ_BUFFER_SIZE];
    memset(pipe_line, 0, READ_BUFFER_SIZE);

    // Read from the pipe as long as the connection with client open
    while(1){
      // Read from your own pipe
      read_line_from_pipe(users_rec.login, pipe_line, READ_BUFFER_SIZE);
      // Dump the read line into the socket
      send_to_client(sock, pipe_line);
      send_to_client(sock, "\n");
      memset(pipe_line, 0, READ_BUFFER_SIZE);
    }
    exit(0);
  }
  /*********************** READER PROCESS ENDS*************************/


  /********************* CONTROLLER PROCCESS **************************/
  fprintf(stderr, "serving %s\n", users_rec.login);
  // Some necessary decalarations
  int charsRead = 0;
  char line[READ_BUFFER_SIZE];
  memset(line, 0, READ_BUFFER_SIZE);
  pid_t pid_bomb = 0;

  // Need to fetch the enviroment variable
    // Extracting the enviroment variable
  int TIME_OUT;
  const char * env_var_time = getenv("TIME_OUT");
  if(env_var_time == NULL)
    TIME_OUT = 1800; // In case was not set, set to 30 min
  else
    TIME_OUT = atoi(env_var_time);

  // Timer, we need to log the user out correctly 
  if((pid_bomb = horrible_timer(TIME_OUT)) == 0){
    // Make a record into the database before logout
    time_t time_sec;
    time(&time_sec);
    users_rec.last_logout_time = time_sec;
    users_rec.logged_in = 0;
    write_UDBRec_from_file(&users_rec, offset);
    fprintf(stderr, "closing connection with %s\n", ip_address);
    close(sock);
    return;
  }
    
  // Read lines from socket in an infinite loop
  while((charsRead = sreadLine(sock, line, READ_BUFFER_SIZE - 1)) > 0){
    // Kill the time bomb process
    kill(pid_bomb, SIGKILL); 
    
    // If user sends '\n'
    if((*line) == '\0'){
      if((pid_bomb = horrible_timer(TIME_OUT)) == 0) break; 
      continue;
    }

    // Respond to the command
    if(interpret(sock, line, &users_rec)) break; // COMMAND_LOGOUT
    // Clean
    memset(line, 0, READ_BUFFER_SIZE);
    
    // Set a new time bomb
    if((pid_bomb = horrible_timer(TIME_OUT)) == 0) break; 
  }

  // The case when client closes the connection
  if(charsRead <= 0) kill(pid_bomb, SIGKILL);
  
  // Make a record into the database before logout
  time_t time_sec;
  time(&time_sec);
  users_rec.last_logout_time = time_sec;
  users_rec.logged_in = 0;
  write_UDBRec_from_file(&users_rec, offset);

  // Kill the reader process
  kill(pid_reader, SIGKILL); // kill the child

  /*
  UsersDB users_db;
  FILE * users_db_file1 = fopen(DATABASE_NAME, "rb");
  load_db_from_file(users_db_file1, &users_db);
  read_usersDB(&users_db, stderr);
  fclose(users_db_file1);  
  */

  fprintf(stderr, "closing connection with %s\n", ip_address);
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

// Returns the pid of the bomb. Kill it before it kills you.
pid_t horrible_timer(int sec){
  
  // We need a process that continously reads from owner's pipe.
  pid_t pid_timebomb;
  // Die in case fork fails
  if((pid_timebomb = fork()) < 0){
    return -1;  // Should not die necessarily. 
  }

  // The time bomb below
  if(pid_timebomb == 0){
    sleep(sec);
    kill(getppid(), SIGKILL);
    return 0;
  }
  return pid_timebomb;
}

