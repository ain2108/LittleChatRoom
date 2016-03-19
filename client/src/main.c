#include "login.h"

int main(){

  FILE * socket = fopen("test.txt", "w");
  send_login(socket);
  fclose(socket);
  return 0;
}
