/**
 * AVR Slideshow
 * by Peter Varga
 */

#ifdef NO_AVR
#include <stdio.h>
#include "buttonsim.h"
#include "lcdsim.h"
#include <time.h>
#else
#include "avr/io.h"
#include "lcd.h"
#endif // NO_AVR

#include <stdint.h>
#include <stdlib.h>
#include "charmap.h"
#include "utils.h"

#ifndef NO_AVR
#define __AVR_ATMEGA128__ 1

#define CG_RAM_ADDR       0x40

#define BUTTON_NONE   0
#define BUTTON_CENTER 1
#define BUTTON_LEFT   2
#define BUTTON_RIGHT  3
#define BUTTON_UP     4
#define BUTTON_DOWN   5
#endif // NO_AVR

#define MAX_LEN 128
#define LCD_WIDTH 16


#ifndef NO_AVR
// TIMER-BASED RANDOM NUMBER GENERATOR ---------------------------------------

static void rnd_init() {
  TCCR0 |= (1  << CS00);  // Timer 0 no prescaling (@FCPU)
  TCNT0 = 0;              // init counter
}

// generate a value between 0 and max
static int rnd_gen(int max) {
  return TCNT0 % max;
}
#else

static void rnd_init() {
    srand(time(NULL));
}

static int rnd_gen(int max) {
    return rand() % max;
}

#endif

#ifndef NO_AVR
// GENERAL INIT - USED BY ALMOST EVERYTHING ----------------------------------

static void port_init() {
  PORTA = 0b00000000; DDRA = 0b01000000; // buttons
  PORTB = 0b00000000; DDRB = 0b00000000;
  PORTC = 0b00000000; DDRC = 0b11110111; // lcd
  PORTD = 0b11000000; DDRD = 0b00001000;
  PORTE = 0b00100000; DDRE = 0b00110000; // buzzer
  PORTF = 0b00000000; DDRF = 0b00000000;
  PORTG = 0b00000000; DDRG = 0b00000000;
}

// SOUND GENERATOR -----------------------------------------------------------

typedef struct {
    int freq;
    int length;
} tune_t;

static tune_t TUNE_START[] = { { 2000, 40 }, { 0, 0 } };
static tune_t TUNE_LEVELUP[] = { { 3000, 20 }, { 0, 0 } };
static tune_t TUNE_SHOOT[] = { { 5000, 10 }, { 0, 0 } };
static tune_t TUNE_GAMEOVER[] = { { 1000, 200 }, { 1500, 200 }, { 2000, 400 }, { 0, 0 } };

static void play_note(int freq, int len) {
    for (int l = 0; l < len; ++l) {
        int i;
        PORTE = (PORTE & 0b11011111) | 0b00010000;	//set bit4 = 1; set bit5 = 0
        for (i=freq; i; i--);
        PORTE = (PORTE | 0b00100000) & 0b11101111;	//set bit4 = 0; set bit5 = 1
        for (i=freq; i; i--);
    }
}

static void play_tune(tune_t *tune) {
    while (tune->freq != 0) {
        play_note(tune->freq, tune->length);
        ++tune;
    }
}

// BUTTON HANDLING -----------------------------------------------------------
static int button_accept = 1;

static int button_pressed() {
  // up
  if (!(PINA & 0b00000001) & button_accept) { // check state of button 1 and value of button_accept
    button_accept = 0; // button is pressed
    return BUTTON_RIGHT;
  }

  // left
  if (!(PINA & 0b00000010) & button_accept) { // check state of button 2 and value of button_accept
    button_accept = 0; // button is pressed
    return BUTTON_UP;
  }

  // center
  if (!(PINA & 0b00000100) & button_accept) { // check state of button 3 and value of button_accept
    button_accept = 0; // button is pressed
    return BUTTON_CENTER;
  }

  // right
  if (!(PINA & 0b00001000) & button_accept) { // check state of button 4 and value of button_accept
    button_accept = 0; // button is pressed
    return BUTTON_DOWN;
  }

  // down
  if (!(PINA & 0b00010000) & button_accept) { // check state of button 5 and value of button_accept
    button_accept = 0; // button is pressed
    return BUTTON_LEFT;
  }

  return BUTTON_NONE;
}

static void button_unlock() {
  //check state of all buttons
  if (
    ((PINA & 0b00000001)
    |(PINA & 0b00000010)
    |(PINA & 0b00000100)
    |(PINA & 0b00001000)
    |(PINA & 0b00010000)) == 31)
  button_accept = 1; //if all buttons are released button_accept gets value 1
}

static int button_released() {
    button_unlock();
    return button_accept;
}

// LCD HELPERS ---------------------------------------------------------------

static void lcd_delay(unsigned int a) {
  while (a)
    a--;
}

static void lcd_pulse() {
  PORTC = PORTC | 0b00000100; //set E to high
  lcd_delay(1400);          //delay ~110ms
  PORTC = PORTC & 0b11111011; //set E to low
}

static void lcd_send(int command, unsigned char a) {
  unsigned char data;

  data = 0b00001111 | a;                //get high 4 bits
  PORTC = (PORTC | 0b11110000) & data;  //set D4-D7
  if (command)
    PORTC = PORTC & 0b11111110;         //set RS port to 0 -> display set to command mode
  else
    PORTC = PORTC | 0b00000001;         //set RS port to 1 -> display set to data mode
  lcd_pulse();                          //pulse to set D4-D7 bits

  data = a<<4;                          //get low 4 bits
  PORTC = (PORTC & 0b00001111) | data;  //set D4-D7
  if (command)
    PORTC = PORTC & 0b11111110;         //set RS port to 0 -> display set to command mode
  else
    PORTC = PORTC | 0b00000001;         //set RS port to 1 -> display set to data mode
  lcd_pulse();                          //pulse to set d4-d7 bits
}

static void lcd_send_command(unsigned char a) {
  lcd_send(1, a);
}

static void lcd_send_data(unsigned char a) {
  lcd_send(0, a);
}

static void lcd_init() {
  //LCD initialization
  //step by step (from Gosho) - from DATASHEET

  PORTC = PORTC & 0b11111110;

  lcd_delay(10000);

  PORTC = 0b00110000;   //set D4, D5 port to 1
  lcd_pulse();          //high->low to E port (pulse)
  lcd_delay(1000);

  PORTC = 0b00110000;   //set D4, D5 port to 1
  lcd_pulse();          //high->low to E port (pulse)
  lcd_delay(1000);

  PORTC = 0b00110000;   //set D4, D5 port to 1
  lcd_pulse();          //high->low to E port (pulse)
  lcd_delay(1000);

  PORTC = 0b00100000;   //set D4 to 0, D5 port to 1
  lcd_pulse();          //high->low to E port (pulse)

  lcd_send_command(DISP_ON);    // Turn ON Display
  lcd_send_command(CLR_DISP);   // Clear Display
}

static void lcd_send_text(char *str) {
  while (*str)
    lcd_send_data(*str++);
}

static void lcd_send_line1(char *str) {
  lcd_send_command(DD_RAM_ADDR);
  lcd_send_text(str);
}

static void lcd_send_line2(char *str) {
  lcd_send_command(DD_RAM_ADDR2);
  lcd_send_text(str);
}
#endif // NO_AVR


// SPEED LEVELS --------------------------------------------------------------

typedef struct {
    int	delay;
    int kills;
} level_t;

#define LEVEL_NUM 6
static level_t LEVELS[] = { { 30, 5 }, { 25, 10 }, { 20, 15 }, { 15, 20 }, { 10, 30 }, { 0, 0 } };
static int level_current = 0;
static int delay_cycle;

static void kill_allien() {
    // do nothing if already at top speed
    if (level_current == LEVEL_NUM-1)
        return;

    // if enough rows removed, increase speed
    if (--LEVELS[level_current].kills == 0) {
        ++level_current;
#ifndef NO_AVR
        play_tune(TUNE_LEVELUP);
#endif
    }
}

// GRAPHICS ------------------------------------------------------------------

// used for visualization of movements
#define CHAR_PATTERN_CANNON_1       0
#define CHAR_PATTERN_CANNON_2  		1
#define CHAR_PATTERN_ALIEN			2
#define CHAR_PATTERN_ALIEN_MISSILE	3
#define CHAR_PATTERN_MISSILE		4
#define CHAR_PLAYGROUND_EMPTY		5
#define CHAR_PLAYGROUND_PATTERN		6
#define CHAR_PLAYGROUND_PLAYGROUND	7
#define CHAR_EMPTY_EMPTY			' '
#define CHAR_ERROR					'X'

#define CHARMAP_SIZE 8
static unsigned char CHARMAP[CHARMAP_SIZE][8] = {
    { 0b00011, 0b00011, 0b01111, 0b00011, 0b00011, 0, 0, 0 },					// CHAR_PATTERN_CANNON_1
    { 0, 0, 0, 0, 0, 0, 0, 0 },                                                 // CHAR_PATTERN_CANNON_2
    { 0b10100, 0b01110, 0b11011, 0b11101, 0b11101, 0b11011, 0b01110, 0b10100 },	// CHAR_PATTERN_ALIEN
    { 0b10101, 0b01010, 0b10101, 0b01010, 0b10101, 0b01010, 0b10101, 0b01010 },	// CHAR_PATTERN_PATTERN
    { 0b11111, 0b11111, 0b11111, 0b11111, 0b10101, 0b01010, 0b10101, 0b01010 },	// CHAR_PATTERN_PLAYGROUND
    { 0, 0, 0, 0, 0b11111, 0b11111, 0b11111, 0b11111 },							// CHAR_PLAYGROUND_EMPTY
    { 0b10101, 0b01010, 0b10101, 0b01010, 0b11111, 0b11111, 0b11111, 0b11111 },	// CHAR_PLAYGROUND_PATTERN
    { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 }	// CHAR_PLAYGROUND_PLAYGROUND
};

#define ALIEN_PLAYGROUND 15
#define DEAD_ZONE 14

static unsigned char ROW1[] = {
    CHAR_PATTERN_ALIEN,
    CHAR_PATTERN_ALIEN,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_PATTERN_CANNON_1
};

static unsigned char ROW2[] = {
    CHAR_PATTERN_ALIEN,
    CHAR_PATTERN_ALIEN,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_EMPTY_EMPTY,
    CHAR_PATTERN_CANNON_2
};

static void chars_init() {
    for (int c = 0; c < CHARMAP_SIZE; ++c) {
        lcd_send_command(CG_RAM_ADDR + c*8);
        for (int r = 0; r < 8; ++r)
            lcd_send_data(CHARMAP[c][r]);
    }
}

static void chars_cannon_rewrite() {
    for (int c = 0;  c < 2; ++c) {
        lcd_send_command(CG_RAM_ADDR + c*8);
        for (int r = 0; r < 8; ++r)
            lcd_send_data(CHARMAP[c][r]);
    }
}

static void screen_update() {
    lcd_send_command(DD_RAM_ADDR);		//set to Line 1

    for (int r1 = 0; r1 < LCD_WIDTH; ++r1) {
        lcd_send_data(ROW1[r1]);
    }

    lcd_send_command(DD_RAM_ADDR2);		//set to Line 2

    for (int r2 = 0; r2 < LCD_WIDTH; ++r2) {
        lcd_send_data(ROW2[r2]);
    }
}

static void move_aliens();

static void new_invaders() {
    if (ROW1[0] == CHAR_PATTERN_ALIEN || ROW2[0] == CHAR_PATTERN_ALIEN)
        move_aliens();

    ROW1[0] = CHAR_PATTERN_ALIEN;
    ROW2[0] = CHAR_PATTERN_ALIEN;
}

static int is_in_dead_zone() {
    if (ROW1[DEAD_ZONE] == CHAR_PATTERN_ALIEN ||
            ROW2[DEAD_ZONE] == CHAR_PATTERN_ALIEN)
        return 1;
    return 0;
}

static void move_aliens() {
    for (int i = ALIEN_PLAYGROUND - 1; i > 0; --i) {
        ROW1[i] = ROW1[i - 1];
        ROW2[i] = ROW2[i - 1];
    }
    ROW1[0] = CHAR_EMPTY_EMPTY;
    ROW2[0] = CHAR_EMPTY_EMPTY;
}

static void move_left() {
    move_aliens();
    ROW2[0] = CHAR_ERROR;
    int *elem = &CHARMAP[1][7];
    for (int i = 16; i > 0; --i) {
        // we are at the edge of the left side
        if (*elem != 0)
            break;
        if (*(elem - 1) != 0) {
            *elem = *(elem - 1);
        }
    }
    chars_cannon_rewrite();
}

static void move_right() {
    move_aliens();
    ROW1[0] = CHAR_ERROR;
}

static void shoot() {
    move_aliens();
    ROW1[0] = CHAR_ERROR;
    ROW2[0] = CHAR_ERROR;
}

static void reset_play_field() {
    for (int i = 0; i < ALIEN_PLAYGROUND; ++i) {
        ROW1[i] = CHAR_EMPTY_EMPTY;
        ROW2[i] = CHAR_EMPTY_EMPTY;
    }
    new_invaders();
    new_invaders();
}

// MAIN ENTRY POINT ----------------------------------------------------------

int main(void)
{
#ifdef NO_AVR
    button_sim_init();
#else
    port_init();
#endif
    lcd_init();

    chars_init();
    rnd_init();

    lcd_send_line1("  AVR Invaders  ");
    lcd_send_line2("  by David Havas");

    while (1) {

        while (button_pressed() != BUTTON_CENTER)
            button_unlock();

        reset_play_field(); // set up new playfield
        delay_cycle = 0; // start the timer
#ifndef NO_AVR
        //play_tune(TUNE_START); // play a start signal
#endif

        // loop of the game
        while (1) {
            // if enough time passed, try to drop the current piece by one row
            if (++delay_cycle > LEVELS[level_current].delay) {
                delay_cycle = 0;
                // move aliens down one row
                move_aliens();
            }

            if (is_in_dead_zone())
                break;

            // if trying to move left or right, do so only if the piece does not leave or collide with the playfield
            int button = button_pressed();
            if (button == BUTTON_LEFT)
                move_left();
            if (button == BUTTON_RIGHT)
                move_right();
            if (button == BUTTON_CENTER)
                shoot();


            // once all movements are done, update the screen
            screen_update();

            // try to unlock the buttons (technicality but must be done)
            button_unlock();
        } // end of game-loop

        // playing some funeral tunes and displaying a game over screen
#ifndef NO_AVR
        //play_tune(TUNE_GAMEOVER);
#endif
        lcd_send_line1("    GAME OVER   ");
        lcd_send_line2("Click to restart");
    }

#ifdef NO_AVR
    button_sim_terminate();
    lcd_sim_terminate();
#endif

    return 0;
}

