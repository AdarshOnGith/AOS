// Author: Adarsh Sharma on 30/08/2024.

#include "header_files.h"
#ifndef UNTITLED_EXECUTE_COMMAND_H
#define UNTITLED_EXECUTE_COMMAND_H

void updateDisplayDir();
void echoHandler(char *tokens[], int num);
char *removeQuotes(const char *str);
void cdHandler(char *token[]);
void pwdHandler();
void processInput(char *input, int bg);
#endif //UNTITLED_EXECUTE_COMMAND_H
