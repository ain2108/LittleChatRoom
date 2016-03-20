#include "db_operations.h"

void load_db_from_file(FILE * db_file, UsersDB * db){
  fread(db, 1, sizeof(UsersDBRec) * N_USERS, db_file);
  return;
}
  
