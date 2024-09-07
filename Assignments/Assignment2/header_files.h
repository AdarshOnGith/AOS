// Author: Adarsh Sharma on 30/08/2024.

#ifndef HEADERS_FILES_H
#define HEADERS_FILES_H

// Standard Library Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// System Headers
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <termios.h>

// Defined Headers
#include "utils.h"
#include "ls.h"
#include "pinfo.h"
#include "history_helper.h"
#include "parse_commands.h"
#include "execute_command.h"
#include "process_handler.h"
#include "additional_feature.h"

// Defined constants
#define BUFF_SIZE 5000
#define MAX_JOBS 10
#define HISTORY_BUFF 1000

typedef struct {
    pid_t pid;            // Process ID
    char command[256];   // Command that started the job
    int isForeground;    // Whether this job is a foreground job
    int isStopped;       // Whether this job is stopped
} Job;


//Global Var
extern char homeDir[BUFF_SIZE];
extern char currDir[BUFF_SIZE];
extern char displayDir[BUFF_SIZE];
extern char prv_address[BUFF_SIZE];
extern char *myShellName;
extern int termOpen;
extern int arrcnt;
extern int downEffect;
extern int upEffect;
extern Job jobs[MAX_JOBS];
extern int jobCount;
extern pid_t foregroundPid;

#endif // HEADERS_FILES_H