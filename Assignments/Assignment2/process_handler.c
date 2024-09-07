// Author: Adarsh Sharma on 30/08/2024.

#include "header_files.h"
#include "process_handler.h"
#include "ctype.h"
#include <signal.h>

// ***** Process_handling*******
// signal - sends signal to a process
// waitpid - makes the current process wait
// getpid - gets the pid of the calling process
// kill - can be used to send any signal to the process or process group

// Function to add a job to the job list
void addJob(pid_t pid, const char *command, int isForeground) {
    if (jobCount < MAX_JOBS) {
        jobs[jobCount].pid = pid;
        strncpy(jobs[jobCount].command, command, sizeof(jobs[jobCount].command) - 1);
        jobs[jobCount].isForeground = isForeground;
        jobs[jobCount].isStopped = 0;
        jobCount++;
    }
}

// Signal handler for SIGTSTP
void sigtstpHandler(int sig) {
    if (foregroundPid == -1) {
        // No foreground job to handle
        return;
    }

    // Stop the foreground job
    if (kill(foregroundPid, SIGTSTP) == 0) {
        printf("\nJob [%d] %d stopped\n", jobCount + 1, foregroundPid);
        displayShellName();
        fflush(stdout);
        // Update job list
        for (int i = 0; i < jobCount; i++) {
            if (jobs[i].pid == foregroundPid) {
                jobs[i].isStopped = 1;
                jobs[i].isForeground = 0; // Move to background
                break;
            }
        }
        foregroundPid = -1; // No foreground job
    } else {
        // perror("\nFailed to stop job");
    }
}


void creatProcess(char *tokens[], int num, int bg) {
    char *cmd = strdup(tokens[0]);
    char *argv[num + 1];
    for (int i = 0; i < num; i++) {
        argv[i] = strdup(tokens[i]);
    }
    argv[num] = NULL;
    int rc = fork();
    if (rc < 0) {
        perror("creating the child process has failed\n");
    } else if (rc == 0) {
        // if bg the child process is now in a new session with no terminal
        if (bg) {
            setpgid(0, 0);
        }
        if (execvp(cmd, argv) == -1) {
            printf("invalid command\n");
            exit(1);
        }
    } else if (rc > 0) {
            addJob(rc, cmd, 1);
            foregroundPid = rc; // Set as the current foreground job
        if (!bg) {
            //printf("gonna wait \n");
            waitpid(rc, NULL, 0);

        } else {
            printf("child with pid [%d] has been sent to the background\n", rc);
        }
    }

}


// Function to check and clean up zombie processes
void checkForZombieProcess() {
    int status;
    int reaped_rc;
    struct dirent *dir_stuff;
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("cannot access the /proc");
        closedir(dir);
        return;
    }

    int pids[BUFF_SIZE];
    char *names[BUFF_SIZE];
    int child_count = 0;

    // Scan the /proc directory for child processes
    while ((dir_stuff = readdir(dir)) != NULL) {
        if (isdigit(dir_stuff->d_name[0])) {
            char path[1000];
            snprintf(path, sizeof(path), "/proc/%s/stat", dir_stuff->d_name);

            FILE *f = fopen(path, "r");
            if (f == NULL) {
                continue;
            }

            int pid, ppid;
            char state;
            char name[BUFF_SIZE];
            if (fscanf(f, "%d %s %c %d", &pid, name, &state, &ppid) == 4) {
                if (ppid == getpid()) {
                    if (child_count < BUFF_SIZE) {
                        pids[child_count] = pid;
                        names[child_count] = strdup(name); // Copy process name
                        child_count++;
                    }
                }
            }
            fclose(f);
        }
    }
    closedir(dir);

    // Reap all terminated child processes
    while ((reaped_rc = waitpid(-1, &status, WNOHANG)) > 0) {
        char stat[200];
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            // snprintf(stat, sizeof(stat), "normally with the status %d", exit_status);
        } else if (WIFSIGNALED(status)) {
            int term_signal = WTERMSIG(status);
            // snprintf(stat, sizeof(stat), "because of the signal %d", term_signal);
        } else if (WIFSTOPPED(status)) {
            int stop_signal = WSTOPSIG(status);
            // snprintf(stat, sizeof(stat), "stopped by signal %d", stop_signal);
        } else {
            snprintf(stat, sizeof(stat), "exited in an unknown manner!");
        }

        char text[BUFF_SIZE];
        char *name = "unknown";
        for (int i = 0; i < child_count; i++) {
            if (pids[i] == reaped_rc) {
                name = names[i];
                break;
            }
        }
        int len = snprintf(text, sizeof(text), "process %s (PID %d) has exited %s\n", name, reaped_rc, stat);
        write(STDERR_FILENO, text, len);
        fflush(stdout);
    }

    // Free dynamically allocated memory
    for (int i = 0; i < child_count; i++) {
        free(names[i]);
    }
}

void checkAndKillBgProcess() {
    int status;
    struct dirent *dir_stuff;
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        //perror("cannot access /proc");
        closedir(dir);
        return;
    }
    int child = 0;
    while ((dir_stuff = readdir(dir)) != NULL) {
        if (isdigit(dir_stuff->d_name[0])) {
            char add[1000];
            sprintf(add, "/proc/%s/stat", dir_stuff->d_name);
            FILE *f = fopen(add, "r");
            if (f == NULL) {
                continue;
            }
            int pid, ppid;
            char state;
            char name[BUFF_SIZE];
            fscanf(f, "%d %s", &pid, name);
            fscanf(f, " %c ", &state);
            fscanf(f, " %d", &ppid);
            if (ppid == (int) getpid()) {
                child++;
                kill(pid, SIGKILL);
            }
            fclose(f);
        }
    }
    closedir(dir);
    while (waitpid(-1, &status, WNOHANG) > 0) {
    }
}