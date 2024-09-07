// Author: Adarsh Sharma on 30/08/2024.

#ifndef UNTITLED_PROCESS_HANDLER_H
#define UNTITLED_PROCESS_HANDLER_H
void creatProcess(char *tokens[], int num, int bg);
void checkForZombieProcess();
void checkAndKillBgProcess();
void sigtstpHandler(int sig);
void addJob(pid_t pid, const char *command, int isForeground);
#endif //UNTITLED_PROCESS_HANDLER_H
