#include "read_write.h"

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

   memset(buffer, 0, READ_BUFFER_SIZE);
   //char terminator[0];
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
