#include "read_write.h"

// This function never returns. Make sure its run in separate proccess.
void f_continous_reading(FILE * from, FILE * to){

  char buffer[READ_BUFFER];
  while(1){
    memset(buffer, 0, READ_BUFFER); // Not to bother with '\0'
    fread(buffer, 1, READ_BUFFER - 1, from); // -1 for '\0'
    fwrite(buffer, 1, strlen(buffer) + 1, to);
  }

  return; // NEVER REACHED
}


void s_continous_reading(int from, int to){

  char buffer[READ_BUFFER];
  int bytesReceived;
  FILE * toFile = fdopen(to, "w");
  memset(buffer, 0, READ_BUFFER); // Not to bother with '\0'

  while((bytesReceived = recv(from, buffer, READ_BUFFER - 1, 0)) > 0){

    fwrite(buffer, 1, READ_BUFFER, toFile);
    memset(buffer, 0, READ_BUFFER); // Not to bother with '\0'
  }

  return; // NEVER REACHED
}

// Reads a line into buffer, no \n char 
int readLine(FILE * sockf, char * buffer, size_t buffSize){
  memset(buffer, 0, buffSize);
  size_t charRead = getline(&buffer, &buffSize, sockf);
  if(charRead < 1) return -1; // In case of an error or EOF
  buffer[charRead - 1] = '\0'; // Getting rid of '\n'
  return charRead - 1; // CARE HERE
}

// Reads a line into a buffer from socket. NULL terminates it.
int sreadLine(int socket, char * buffer, size_t buffSize){

   char character[1];
   int bytesRead = 0;
   int bytERead = 0;
   while(bytesRead < buffSize){

     if( (bytERead = recv(socket, character, 1, 0)) == 0) return 0; // EOF
     else if(bytERead == -1) return -1; // Error
     
     strcpy(buffer + bytesRead, character);
     bytesRead++;
     if(!strcmp(character, "\n")){
       *(buffer + bytesRead - 1) = '\0';
       return bytesRead - 1; // Subtract '\0' 
     }
   }
   return -2; // To indicate that the buffer was too large
}
