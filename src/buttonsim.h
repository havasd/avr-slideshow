#ifndef BUTTONSIM_H
#define BUTTONSIM_H

#define BUTTON_NONE     0
#define BUTTON_CENTER   1
#define BUTTON_LEFT     2
#define BUTTON_RIGHT    3
#define BUTTON_UP       4
#define BUTTON_DOWN     5
#define BUTTON_QUIT    99

void buttonsim_init();
void buttonsim_terminate();

int button_pressed();
void button_unlock();

#endif // BUTTONSIM_H

