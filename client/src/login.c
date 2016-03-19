#include "login.h"

// This function sends login information through the socket
int send_login(FILE * socket){

  // Getting the username
  printf("Username: ");
  char username[USERNAME_LENGTH];
  if(fgets(username, USERNAME_LENGTH, stdin) == NULL)
    die_verbosely("fgets() failed");
  username[strlen(username) - 1] = '\0'; // Getting rid of \n 

  // Getting the password
  char * password;
  password = getpass("Password: ");
  // printf("Your password: %s, length: %d\nUsername length: %d",
  // 	 password, strlen(password), strlen(username));

  // Producing a hash
  unsigned char hash_bin[SHA_DIGEST_LENGTH];
  SHA1((const unsigned char *)password, sizeof(password), hash_bin);
  // free(password);
  // Convert hash to hex
  char hash[41]; 
  int i;
  for(i = 0; i < 20; i++){
    snprintf(hash + (i * 3) , 4, "%02x", hash_bin[i]);
  }

  // Writing to the socket
  fwrite(username, 1 , strlen(username), socket);
  fprintf(socket, " ");
  fwrite(hash, 1 , sizeof(hash), socket);
  fprintf(socket, "\r\n");

  // Reading response

  return 1;
}

// Function opens a socket and connects it to the server
int open_connection(char * servIP, char * servPort){

  int sock;
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    die_verbosely("socket() failed");
  }

  struct sockaddr_in servAddr;
  unsigned short port = atoi(servPort);

  // Filling in the sockaddr structure
  memset(&servAddr, 0, sizeof(servAddr));       // Initiate all bytes to 0
  servAddr.sin_family = AF_INET;                // Internet adress famly
  servAddr.sin_addr.s_addr = inet_addr(servIP); // IP adress
  servAddr.sin_port = htons(port);              // Port

  // Connect the socket to the server
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
    die_verbosely("connect() failed");
  }

  return sock;
}

// Attempts to login the user
int login(FILE * sockf_read, FILE * sockf_write,
	  char * read_buffer, char * write_buffer){

  /* The loop will be sending login information until a succesful login
     or until the server closes the connection */
  while(1){
    send_login(sockf_write);
    int charRead = readLine(sockf_read, read_buffer, READ_BUFFER_SIZE - 1);

    if(charRead < 1){
      fprintf(stdout, "Connection closed by the server.\n");
      return 0; // Connection closed by server
    }else{
      if(!strcmp(read_buffer, "OK")){
	 return 1; // Succesful login
      }else{
	fprintf(stdout, "Wrong username or password. Try again.\n");
	continue;
      }
    }
    
  }
}


