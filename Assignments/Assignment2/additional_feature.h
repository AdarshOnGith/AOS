// Author: Adarsh Sharma on 3/09/2024.

#ifndef UNTITLED_ADDITIONAL_FEATURE_H
#define UNTITLED_ADDITIONAL_FEATURE_H
void checkSetTerminalMode(struct termios *orgMode);
void checkResetTerminalMode(struct termios *orgMode);
void tabKeyHandling(char *buffer, int buflen);
void handArrwKeys(char * escp_seq, char * buffer);
#endif //UNTITLED_ADDITIONAL_FEATURE_H
