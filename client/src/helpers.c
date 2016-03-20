#include "helpers.h"

void die_verbosely(char * error){
  perror(error);
  exit(1);
}
