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

  // READ/WRITE buffers on the heap.
  char * read_buffer = (char *) malloc(sizeof(char) * READ_BUFFER_SIZE);
  char * write_buffer = (char *) malloc(sizeof(char) * WRITE_BUFFER_SIZE);
  memset(read_buffer, 0, READ_BUFFER_SIZE);
  memset(write_buffer, 0, WRITE_BUFFER_SIZE);

  // Attempt login, if successful enter the loop.
  if( login(sock, read_buffer, write_buffer) ){

    // Will fork here to separate the reader process from command input
    int pid_reader;
    if((pid_reader = fork()) < 0){
      die_verbosely("fork() failed");
    }

    /**********************READER PROCESS******************************/
    if(pid_reader == 0){
      
      // To prevent the child stdin from interfering with parent
      int file = open("/dev/null", O_RDONLY);
      dup2(0, file);     
 
      // Read a line, print it to stdout. Repeat until EOF (socket closes
      while(sreadLine(sock, read_buffer, READ_BUFFER_SIZE - 1)){
	fprintf(stderr, "Server: %s\n", read_buffer);
      }
      
      // Cleaning up
      free(read_buffer);
      free(write_buffer);
      return 0;
    }
    /*********************READER PROCESS ENDS**************************/

    /**********************CONTROLLER PROCESS*************************/

    // Read a line from stdin
    size_t size = WRITE_BUFFER_SIZE - 1;
    memset(write_buffer, 0, WRITE_BUFFER_SIZE);
    while(getline(&write_buffer, &size, stdin) ){
      send(sock, write_buffer, strlen(write_buffer), 0);
      memset(write_buffer, 0, WRITE_BUFFER_SIZE);
    }

    // Cleaning up
    free(read_buffer);
    free(write_buffer);
    return 0;
    /********************CONTROLLER PROCESS ENDS**********************/
  }

  // Cleaning up
  free(read_buffer);
  free(write_buffer);

  close(sock);
  return 0;
}

