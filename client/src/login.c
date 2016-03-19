#include "login.h"

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
  printf("Your password: %s, length: %d\nUsername length: %d",
	 password, strlen(password), strlen(username));

  // Producing a hash
  unsigned char hash_bin[SHA_DIGEST_LENGTH];
  SHA1((const unsigned char *)password, sizeof(password), hash_bin);
  free(password);
  // Convert hash to hex
  char hash[41]; 
  int i;
  for(i = 0; i < 20; i++){
    snprintf(hash + (i * 3) , 4, "%02x", hash_bin[i]);
  }

  // Writing to the socket
  fwrite(username, 1 , strlen(username), socket);
  fprintf(socket, "\r\n");
  fwrite(hash, 1 , sizeof(hash), socket);
  fprintf(socket, "\r\n\r\n");

  // Reading response

  return 1;
}
