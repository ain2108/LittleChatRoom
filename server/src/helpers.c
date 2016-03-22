#include "helpers.h"

void die_verbosely(char * error){
  perror(error);
  exit(1);
}

// Reads a line into buffer, no \n char
int readLine(FILE * sockf, char * buffer, size_t buffSize){
  memset(buffer, 0, buffSize);
  size_t charRead = getline(&buffer, &buffSize, sockf);
  if(charRead < 1) return -1; // In case of an error or EOF
  buffer[charRead - 1] = '\0'; // Getting rid of '\n'
  return charRead - 1; // CARE HERE
}

void read_usersDB(UsersDB * db, FILE * stream){
int i = 0;
for(i = 0; i < N_USERS; i++){
  fprintf(stream, "ID: %d Username: %s Password: %s\n"
	  "Logged in: %d Last login: %lu Last logout: %lu\n"
	  "IP: %s pipe: %s\n========\n",
	  db->records[i].user_id,
	  db->records[i].login,
	  db->records[i].password,
	  db->records[i].logged_in,
	  db->records[i].last_login_time,
	  db->records[i].last_logout_time,
	  db->records[i].ip,
          db->records[i].pipe_name);
 }
}

// Reads a line into a buffer from socket. NULL terminates it.
int sreadLine(int socket, char * buffer, size_t buffSize){

   memset(buffer, 0, READ_BUFFER_SIZE);
 
   int bytesRead = 0;
   int bytERead = 0;
   while(bytesRead < buffSize){

     if( (bytERead = recv(socket, buffer, 1, 0)) == 0) return 0; // EOF
     else if(bytERead == -1) return -1; // Error
     
     if( (*buffer) == '\n'){
       *buffer = '\0';
       return bytesRead + 1;  
     }
     buffer++;
     bytesRead++;
   }
   return -2; // To indicate that the buffer was too large
}


