#include "helpers.h"

int main(int argc, char ** argv){

  UsersDB * db = (UsersDB *) malloc(sizeof(UsersDBRec) * N_USERS);
  memset(db, 0, sizeof(UsersDB));
  initialize_database(USERS_FILE_NAME, DATABASE_NAME, db);

  FILE * test = fopen(DATABASE_NAME, "rb");
  UsersDB * dbtest = (UsersDB *) malloc(sizeof(UsersDBRec) * N_USERS);
  fread(dbtest, 1, sizeof(UsersDBRec) * N_USERS, test);

  read_usersDB(db, stderr);



  return 0;

}


