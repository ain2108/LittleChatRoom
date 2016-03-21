#ifndef _DATABASE_INTERACTIONS
#define _DATABASE_INTERACTIONS

#include "helpers.h"

// IP DATABASE FUNCTIONS
typedef struct IP_DBRec{
  char IP[16];
  long ban_time;

} IP_DBRec;

void create_IP_DBRec(char * ip_address);
void load_ip_db(char * buffer);
int find_position_in_IPDB(IP_DBRec * ip_array, char * ip_address);
void update_IPRec_in_dbfile(IP_DBRec * record, int offset);
int check_validity(int socket, UsersDB * db);
int authenticate(int sock, char * ip_address);
int match_pass_user(char * username, char * password, UsersDB * db);

// USERSDB FUNCTIONS
void load_db_from_file(FILE * db_file, UsersDB * db);
void write_UDBRec_from_file(UsersDBRec * record, int offset);
void read_UDBRec_from_file(UsersDBRec * record, int offset);
void fillin_UsersDBRec(UsersDBRec * users_rec, char * ip_address);

#endif
