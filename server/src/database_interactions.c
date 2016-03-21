#include "database_interactions.h"

/********* IP DATABASE MANAGMENT *********************/

// Function called in case IP is not in database
void create_IP_DBRec(char * ip_address){
  FILE * ip_db = fopen(IP_BAN_DB_NAME, "ab");
  IP_DBRec record;
  memset(&record, 0, sizeof(IP_DBRec));
  strcpy(record.IP, ip_address);
  time_t time_sec;
  time(&time_sec);
  record.ban_time = 0;

  fwrite(&record, sizeof(IP_DBRec), 1, ip_db);
  fclose(ip_db);
  return;
}

// Function load the contents of db file into an array
void load_ip_db(char * buffer){
  FILE * ip_db = fopen(IP_BAN_DB_NAME, "rb");
  fread(buffer, sizeof(IP_DBRec), MAX_IP_DB_REC_N, ip_db);
  return;
}

// Looks for the position of certain IP_DBRec in the database
int find_position_in_IPDB(IP_DBRec * ip_array, char * ip_address){
  int i = 0;
  while(i < MAX_IP_DB_REC_N){
    if(!strcmp(ip_array[i].IP, ip_address))
      return i;
    i++;
  }
  return -1; // If ip address not in database
}

void update_IPRec_in_dbfile(IP_DBRec * record, int offset){

  FILE * ip_db = fopen(IP_BAN_DB_NAME, "wb");
  fseek(ip_db, offset, SEEK_SET);
  fwrite(record, sizeof(IP_DBRec), 1, ip_db);
  fclose(ip_db);
  return;
}

// Checks username and pass combo. If incorrect, returns 0
int check_validity(int socket, UsersDB * db){

  // Some buffers
  char line[1024];
  char password[1024];
  char username[1024];
  memset(line, 0, 1024);
  memset(password, 0, 1024);
  memset(username, 0, 1024);
  char * word;
  
  // Need to extract username and password from the line
  sreadLine(socket, line, PASS_USRN_LENGTH - 1);
  word = strtok(line, " ");
  strcpy(username, word);
  word = strtok(NULL, "\0");
  strcpy(password, word);
   
  // Check if match occurs
  if(match_pass_user(username, password, db))
    return 1;
  else{
    return 0;
  }
}

// Tries to match username and password provided by the user with db
int match_pass_user(char * username, char * password, UsersDB * db){
  int i = 0;
  while(i < N_USERS){
    if(strcmp(username, db->records[i].login)){
      i++;
      continue;
    }
    if(strcmp(password, db->records[i].password)){
      i++;
      continue;
    }
    return 1;
  }
  return 0;
}

// Authenticates the user. 
int authenticate(int sock, char * ip_address){
 
  int BLOCK_TIME = 180; // FETCH THE ENVIROMENT VAR
  time_t time_sec;
  time(&time_sec);

  fprintf(stderr, "Connection attempted by %s\n", ip_address);
  
  // Assume IP_BAN_DB was created before.
  // Check the ip against database of IP adresses.
  IP_DBRec * ip_db_array
    = (IP_DBRec *) malloc(sizeof(IP_DBRec) * MAX_IP_DB_REC_N);
  memset(ip_db_array, 0, sizeof(IP_DBRec) * MAX_IP_DB_REC_N);
  load_ip_db( (char *) ip_db_array); // db loaded
  int position;
  
  // If ip not found in db, create a new record, write it to db.
  if((position = find_position_in_IPDB(ip_db_array, ip_address)) == -1){
    create_IP_DBRec(ip_address);
    fprintf(stderr, "IP records created and recorded\n");
    load_ip_db( (char *) ip_db_array);
    position = find_position_in_IPDB(ip_db_array, ip_address);
  }

  int offset = position * sizeof(IP_DBRec);
  fprintf(stderr, "IP: %s, position in DB: %d\n", ip_address, position);
  fprintf(stderr, "Time: %d Last banned at: %lu\n",
	  (int) time_sec, ip_db_array[position].ban_time);
  // Check if user is still banned.
  time(&time_sec);
  if((time_sec - ip_db_array[position].ban_time) < BLOCK_TIME){
    fprintf(stdout, "Connection attempted by %s. Still banned\n",
	    ip_address);
    free(ip_db_array);
    close(sock);
    return 0;
  }
  // Reset ban time
  ip_db_array[position].ban_time = 0;
  fprintf(stderr, "ban_time was reset normally\n");

  // Load the database of passwords and usernames
  UsersDB * db = (UsersDB *) malloc(sizeof(UsersDBRec) * N_USERS);
  memset(db, 0, sizeof(UsersDB));
  FILE * db_file = fopen(DATABASE_NAME, "rb");
  fread(db, sizeof(UsersDBRec), N_USERS, db_file);

  // Attempt validation of user. Check for number of tries. 
  int attempts = 0;
  while(attempts < MAX_FAILS){
    if(!check_validity(sock, db)){
      send(sock, "NO\n", 3, 0);
      attempts++; 
    }else{
      return 1;
    }
  }
  time(&time_sec);
  ip_db_array[position].ban_time = time_sec;
  fprintf(stderr, "IP: %s, Banned at %lu", 
	  ip_address, ip_db_array[position].ban_time);
  update_IPRec_in_dbfile(ip_db_array + position, offset);
  close(sock);
  return 0;

}
