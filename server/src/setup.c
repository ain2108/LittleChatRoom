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
  char line[PASS_USRN_LENGTH];
  char * word;
  memset(line, 0, PASS_USRN_LENGTH);
  int charRead = 0;
  
  // Loop to populate the *db
  int i = 0;
  for(i = 0; i < N_USERS; i++){
    charRead = readLine(source, line, PASS_USRN_LENGTH - 1);
    if(charRead < 1) break; // If less users are provided than expected

    // Filling the Rec
    word = strtok(line, " ");   
    db->records[i].user_id = i;
    strcpy(db->records[i].login, word); // Weak code.
    word = strtok(NULL, "\0");
    strcpy(db->records[i].password, word); // Weak code.

    memset(line, 0, PASS_USRN_LENGTH);
    memset(word, 0, PASS_USRN_LENGTH);
  }

  fwrite(db, N_USERS, sizeof(UsersDBRec), db_file);

  fclose(db_file);
  fclose(source);
  
}
