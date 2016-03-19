#include "helpers.h"

// Usage 
int main(int argc, char ** argv){

  // Checking for correct number of arguments
  if(argc != 3){
      fprintf(stderr, "Usage: %s <server_IP_adress> <server_port_no>\n", argv[0]);
      exit(1);
  }

  // Open a socket connection  
  int sock = open_connection(argv[1], argv[2]);
  FILE * sockf_read = fdopen(sock, "r");
  FILE * sockf_write = fdopen(sock, "w");

  // READ/WRITE buffers on the heap.
  char * read_buffer = (char *) malloc(sizeof(char) * READ_BUFFER_SIZE);
  char * write_buffer = (char *) malloc(sizeof(char) * WRITE_BUFFER_SIZE);
  memset(read_buffer, 0, READ_BUFFER_SIZE);
  memset(write_buffer, 0, WRITE_BUFFER_SIZE);

  // Attempt login, if successful enter the loop.
  if( login(sockf_read, sockf_write, read_buffer, write_buffer) ){

    // Will fork here to separate the reader process from command input
    int pid_reader;
    if((pid_reader = fork()) < 0){
      die_verbosely("fork() failed");
    }

    /**********************READER PROCESS******************************/
    if(pid_reader != 0){
      
      // Read a line, print it to stdout. Repeat until EOF (socket closes)
      while(readLine(sockf_read, read_buffer, READ_BUFFER_SIZE - 1)){
	fprintf(stdout, "%s\n", read_buffer);
      }
      
      // Cleaning up
      free(read_buffer);
      free(write_buffer);
      fclose(sockf_read);
      fclose(sockf_write);
      return 0;
    }
    /*********************READER PROCESS ENDS**************************/

    /**********************CONTROLLER PROCESS*************************/

    // Read a line from stdin
    int charToWrite = readLine(stdin, write_buffer, WRITE_BUFFER_SIZE - 1);
    while(charToWrite == fwrite(write_buffer, 1,
				strlen(write_buffer), sockf_write)){
     
      fprintf(sockf_write, "\r\n"); // Complete transmission
      // Read in another line
      charToWrite = readLine(stdin, write_buffer, WRITE_BUFFER_SIZE - 1);
    }

    // Cleaning up
    free(read_buffer);
    free(write_buffer);
    fclose(sockf_read);
    fclose(sockf_write);
    return 0;
    /********************CONTROLLER PROCESS ENDS**********************/
  }

  // Cleaning up
  free(read_buffer);
  free(write_buffer);
  fclose(sockf_read);
  fclose(sockf_write);
  return 0;
}

