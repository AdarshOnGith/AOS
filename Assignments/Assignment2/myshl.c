#include "header_files.h"

struct termios orgMode;
char *myShellName;
char displayDir[BUFF_SIZE];
char homeDir[BUFF_SIZE]; // it ends with the /
char currDir[BUFF_SIZE]; // it ends with the /  

int arrcnt;
int termOpen = 0;
int downEffect = 0;
int upEffect = 0;

Job jobs[MAX_JOBS];
int jobCount = 0;
pid_t  foregroundPid = -1;

// Function to handle job for Ctrl_D control
void handle_Ctrl_D() {
    bool has_stopped_jobs = false;
    for (int i = 0; i < jobCount; i++) {
        if (jobs[i].isStopped) {
            has_stopped_jobs = true;
            break;
        }
    }
    if (has_stopped_jobs) {
        printf("\nexit..\n");
        printf("There are stopped jobs.\n");
        // fflush(stdout);
    }
    else printf("\n");
}

void currentDirFromNewRoot() {
    // Calculate the length of the home directory path
    int actualHomeDirLen = (int) strlen(homeDir);

    // Case 1: If the current directory is shorter than the home directory, it's not within the home directory
    if (strlen(currDir) < (size_t)actualHomeDirLen) {
        // Directly copy current directory to displayDir
        strcpy(displayDir, currDir);

        // Remove trailing slash if it exists
        int len = strlen(displayDir);
        if (len > 0 && displayDir[len - 1] == '/') {
            displayDir[len - 1] = '\0';
        }
        return;
    }

    // Case 2: Check if the current directory starts with the home directory
    for (int i = 0; i < actualHomeDirLen; i++) {
        if (homeDir[i] != currDir[i]) {
            // If current directory does not start with the home directory, copy it as is
            strcpy(displayDir, currDir);
            return;
        }
    }

    // Case 3: Current directory starts with the home directory
    // Construct display path by replacing the home directory with '~/'
    strcpy(displayDir, "~/");

    // Append the part of currDir that comes after homeDir
    strcat(displayDir, currDir + actualHomeDirLen);

    // Remove trailing slash if it exists
    int len = strlen(displayDir);
    if (len > 0 && displayDir[len - 1] == '/') {
        displayDir[len - 1] = '\0';
    }
}


void setNewHomeDir() {
    // Get current working directory
    if (getcwd(homeDir, BUFF_SIZE) == NULL) {
        perror("getcwd failed : unable to fetch directory details for Home Directory");
        _exit(EXIT_FAILURE); // Exit if getcwd fails
    }

    // Ensure the path ends with a trailing slash
    // Check if homeDir is not empty before accessing the last character
    if (strlen(homeDir) > 0 && homeDir[strlen(homeDir) - 1] != '/') {
        if (strlen(homeDir) + 1 < BUFF_SIZE) { // Check buffer overflow possibility
            strcat(homeDir, "/");
        } else {
            perror("Buffer too small to append trailing slash\n");
            _exit(EXIT_FAILURE); // Exit if buffer is too small
        }
    }
}

char *getMyShellName() {
    const int MAX_CREDS_LEN = 2048;
    char usern[MAX_CREDS_LEN];

    // Get login name
    if (getlogin_r(usern, MAX_CREDS_LEN) != 0) {
        perror("Can't get login name");
        return NULL;
    }
    
    // Gets system information
    struct utsname sysName;
    if (uname(&sysName) != 0) {
        perror("uname");
    }

    // Allocate memory for final name for termnal
    char *usr_sysnm = (char *) malloc(500);
    if (usr_sysnm == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Create final name for termnal
    sprintf(usr_sysnm, "%s@%s:", usern, sysName.sysname);
    return usr_sysnm;
}

void mySignalExit(int sigcode ) {
    switch (sigcode) {
        case SIGINT:
            checkAndKillBgProcess();  
            printf("^C\n");
            fflush(stdout); 
            break;
        case SIGTSTP:
            // raise(SIGSTOP); // Suspend the process
            sigtstpHandler(sigcode);
            break;
        case SIGCHLD:
            checkForZombieProcess(); // Handle terminated child processes
            break;
        case SIGCONT:
            // Print message when the process is resumed with SIGCONT
            printf("\nProcess resumed (SIGCONT).\n");
            break;

        default:
            // Handle unexpected signals
            // fprintf(stderr, "Unexpected signal %d received.\n", sigcode);
            break;
    }
}

void checkArr() {
    int  length = 0;
    char buffer[BUFF_SIZE];
    char escp_seq[4];
    char ch;
    while (1) {
        ssize_t bytesRead = read(STDIN_FILENO, &ch, 1);
            
            if (ch == '\004') { // Check if character is Ctrl+D (EOF)
                checkResetTerminalMode(&orgMode);
                handle_Ctrl_D();
                _exit(EXIT_SUCCESS);
            }
            if (bytesRead <= 0) {
                continue;
            }

            if (ch == '\n') {  // Enter key pressed
                buffer[length] = '\0';
                printf("\n");
                arrcnt = 0;
                downEffect = 0;
                // Processing the command
                addHistory(buffer);
                commandParser(buffer);
                buffer[0] = '\0'; // Clear buffer to avoid leftover data
                length = 0;
                break;
            } else if (ch == '\033') {  // Escape sequence start
                escp_seq[0] = ch;
                // Read additional bytes for the escape sequence
                ssize_t n = read(STDIN_FILENO, escp_seq + 1, 2);
                if (n >= 2) {
                    escp_seq[3] = '\0'; // Null-terminate the sequence
                    handArrwKeys(escp_seq, buffer);
                    length = strlen(buffer); 
                }
            }else if (ch == '\t') { 
                    if(length) {
                        tabKeyHandling(buffer, length);
                        length = strlen(buffer); 
                    }
            }else if (ch == '\b' || ch == 127) { // Backspace key (DEL character)
                if (length > 0) {
                    // Erase character from buffer
                    length--;
                    // Move cursor back and erase character on terminal
                    printf("\b \b");
                    fflush(stdout);
                }
            } else {
                if (length < BUFF_SIZE - 1) {
                    buffer[length++] = ch;
                    putchar(ch); // Display the typed character
                    fflush(stdout); // Ensure character is displayed immediately
                }
            }
     }
    checkResetTerminalMode(&orgMode);
}

int main() {
    clearScr();
    // getcolor(1);
    printFirstMsg(); // prints Header Msg only once when shell starts
    myShellName = getMyShellName();
    // reseetcolor();
    if (myShellName == NULL) {
        perror("Error: Failed to get the shell name\n");
        _exit(EXIT_FAILURE);
    }

    setNewHomeDir();
    strcpy(currDir, homeDir);

    //at exit kills bg process
    signal(SIGINT, mySignalExit); // Handle Ctrl+C
    signal(SIGTSTP, mySignalExit); // Handle Ctrl+Z
    signal(SIGCHLD, mySignalExit); // Handle child process termination
    currentDirFromNewRoot();

    while (1) {
        checkSetTerminalMode(&orgMode);
        displayShellName();
        fflush(stdout);
        checkArr();
    }
    checkResetTerminalMode(&orgMode);
    _exit(EXIT_SUCCESS);
}