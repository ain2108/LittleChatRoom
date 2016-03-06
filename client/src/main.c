#include "login.h"

int main(){

  FILE * socket = fopen("test.txt", "w");
  login(socket);
  fclose(socket);
  return 0;
}
