#include "login.h"



int login(FILE * socket){

  // Getting the username
  printf("Username: ");
  char username[USERNAME_LENGTH];
  if(fgets(username, USERNAME_LENGTH, stdin) == NULL){
    printf("fgets() failed");
    exit(1);
  }
  
  // Write to socket
  fwrite(username, 1 , strlen(username), socket);

  // Reading the response

  // Getting the password
  char * password;
  password = getpass("Password: ");
  printf("Your password: %s", password);

  // Producing a hash
  unsigned char hash_bin[SHA_DIGEST_LENGTH];
  SHA1(password, sizeof(password), hash_bin);

  char hash[61]; //null terminator
  int i;
  for (i = 0; i < 20; i++) {
    snprintf(hash + i*3, 4, "%02x", hash_bin[i]);
  }

  // Writing to the socket
  fwrite(hash, 1 , sizeof(hash), socket);

  // Reading response

  return 1;
}
