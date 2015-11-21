#include "buttonsim.h"

#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


static int button_accept = 1;

/* Implementation is based on the following description:
 * http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses
 */

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int enable)
{
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~(ICANON | ECHO);
        ttystate.c_cc[VMIN] = 1;
    } else
        ttystate.c_lflag |= ICANON | ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}


void buttonsim_init()
{
    nonblock(1);
}

void buttonsim_terminate()
{
    nonblock(0);
}

int button_pressed()
{
    if (kbhit()) {
        button_accept = 0;
        char c = fgetc(stdin);
        switch (c) {
        case 'h':
        case '4':
            return BUTTON_LEFT;
        case 'j':
        case '2':
            return BUTTON_DOWN;
        case 'k':
        case '8':
            return BUTTON_UP;
        case 'l':
        case '6':
            return BUTTON_LEFT;

        case ' ':
        case '5':
            return BUTTON_CENTER;
        case 'q':
        case 27:
            return BUTTON_QUIT;

        default:
            return BUTTON_NONE;
        }
    }

    return BUTTON_NONE;
}

void button_unlock()
{
    button_accept = !kbhit();
}
