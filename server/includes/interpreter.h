#ifndef _INTERPRETER_
#define _INTERPRETER

#define COMMAND_WHO "who"
#define COMMAND_LAST "last"
#define COMMAND_BROADCAST "broadcast"
#define COMMAND_SEND "send"
#define COMMAND_LOGOUT "logout"

#include "helpers.h"

int interpret(int socket, char * read_line, UsersDBRec * users_rec);
void perform_WHO(int socket, UsersDB * db);
void perform_LAST(int socket, int min, UsersDB * db);
void perform_BROADCAST(char * message, UsersDBRec * users_rec, UsersDB * db);
void perform_MULTI_SEND(char * receivers_message, UsersDB * db, int socket);
void perform_SEND(char * receiver_message, UsersDB * db, 
		  int socket, UsersDBRec * rec);
int check_username_exits_and_online(char * login, UsersDB * db);

#endif
