#ifndef _INTERPRETER_
#define _INTERPRETER

#define COMMAND_WHO "who"
#define COMMAND_LAST "last"
#define COMMAND_BROADCAST "broadcast"
#define COMMAND_SEND "send"
#define COMMAND_LOGOUT "logout"

#include "helpers.h"

void interpret(int socket, char * read_line, UsersDBRec * users_rec);
void perform_WHO(int socket, UsersDB * db);
void perform_LAST();
void perform_BROADCAST();
void perform_SEND();
void perform_LOGOUT();


#endif
