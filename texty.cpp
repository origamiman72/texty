/*** includes ***/

#include <iostream>  
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
using namespace std; 

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
    // perror prints a descriptive error message.
    perror(s);
    exit(1);
}

void disableRawMode() 
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    }
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    {
        die("tcgetattr");
    }

    // Registers disableRawMode to be run on exit.
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    // Note: the comments describe the effect from disabling these flags.

    // BRKINT: Disables break condition sending SIGINT.
    // ICRNL: Fixes Ctrl-m being read as Enter (Carriage Return New Line).
    // INPCK: Disables Parity Checking.
    // ISTRIP: Stops Stripping 8th bit of each input byte.
    // IXON: Disables Ctrl-s and Ctrl-q.
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP| IXON);

    // OPOST: Turns off terminal output processing (prevents automatic \r).
    raw.c_oflag &= ~(OPOST);

    // (Not Disabled) Sets character size to 8 bits/byte.
    raw.c_cflag |= (CS8);

    // ECHO: Tells terminal not to print user input.
    // ICANON: Turns off Canonical mode.
    // IEXTEN: Disables Ctrl-v.
    // ISIG: Disables Ctrl-c/z
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Minimum number of input bytes before returning.
    raw.c_cc[VMIN] = 0;
    // Time before returning, in tenths of seconds.
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        die("tcsetattr");
    }
}

/*** init ***/
  
int main() 
{ 
    enableRawMode();

    while (1)
    {
        char c = '\0';

        // EAGAIN is the timeout error in Cygwin so we ignore it.
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        {
            die("read");
        }

        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }

    return 0; 
} 