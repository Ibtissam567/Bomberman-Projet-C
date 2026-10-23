#ifndef INPUT_H
#define INPUT_H

typedef struct {
    int p1_up, p1_down, p1_left, p1_right, p1_bomb;
    int p2_up, p2_down, p2_left, p2_right, p2_bomb;
    int quit;
    int pause;
} InputState;

void input_init(void);
void input_restore(void);
void input_flush(void);
void input_poll(InputState *s);
void sleep_ms(int ms);
void play_sfx(int kind); /* 1=bombe 2=explosion 3=bonus 4=mort 5=fin */

#ifdef _WIN32
void console_enable_colors(void);
void console_set_color(int color);
void console_reset_color(void);
void console_hide_cursor(int hide);
void console_home(void);
#else
void console_enable_colors(void);
void console_set_color(int color);
void console_reset_color(void);
void console_hide_cursor(int hide);
void console_home(void);
#endif

/* Couleurs (Windows FOREGROUND_* ou ANSI) */
#define COL_RESET   7
#define COL_WALL    8
#define COL_OBS     14
#define COL_P1      10
#define COL_P2      11
#define COL_BOMB    12
#define COL_BOOM    4
#define COL_POWER   13
#define COL_TITLE   14
#define COL_TEXT    7

#endif /* INPUT_H */
