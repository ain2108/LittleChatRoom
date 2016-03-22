#include "setup.h"

int setup_server_socket(unsigned short port){

  int servSock;
  struct sockaddr_in servAddr; // might need to be malloced

  // Create a socket
  if( (servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_verbosely("socket() failed");

  // Construct a local adress structure
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;                   // Adress family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);    // Any interface
  servAddr.sin_port = htons(port);                 // port

  // Bind to the local adress
  if( (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr))) < 0)
    die_verbosely("bind() failed");

  // Start listening
  if(listen(servSock, MAX_PENDING) < 0)
    die_verbosely("listen() failed");

  return servSock;
}

// Function will initialize the database and write it into db file 
void initialize_database(char * source_file_name, char * db_name, UsersDB * db){

  // Initialize everything to '\0' to be sure
  memset(db, 0, sizeof(UsersDBRec) * N_USERS);

  // Declarations
  FILE * source = fopen(source_file_name, "r");
  FILE * db_file = fopen(db_name, "wb");
  char * line;
  char * word;
  
   // Loop to populate the *db
  int i = 0;
  size_t size = PASS_USRN_LENGTH - 1;
  for(i = 0; i < N_USERS; i++){
    if(getline(&line, &size, source) == -1) break;
    
    // Filling the Rec
    word = strtok(line, " ");
    if(word == NULL) break;
    db->records[i].user_id = i + 1; // ID
    strcpy(db->records[i].login, word); // login
    word = strtok(NULL, "\n");
    strcpy(db->records[i].password, word); // password

    // Creating an appropriate name for the pipe
    strcpy(db->records[i].pipe_name, PIPE_DIRECTORY);
    strcat(db->records[i].pipe_name, db->records[i].login);
    strcat(db->records[i].pipe_name, PIPE_NAME_POSTFIX);
  }
  // Write the entries into the db file
  fwrite(db, N_USERS, sizeof(UsersDBRec), db_file);

  // Clean up
  fclose(db_file);
  fclose(source);
}

// Creates all FIFOs
void create_FIFOs(UsersDB * users_db){
    int i = 0;
    for(i = 0; i < N_USERS; i++){
      mkfifo(users_db->records[i].pipe_name, 0666); 
    }
}

