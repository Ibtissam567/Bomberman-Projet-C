#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include "input.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void input_init(void) {
    console_enable_colors();
    console_hide_cursor(1);
}

void input_restore(void) {
    console_hide_cursor(0);
    console_reset_color();
}

void input_flush(void) {
    while (_kbhit()) _getch();
}

void sleep_ms(int ms) {
    Sleep((DWORD)ms);
}

void play_sfx(int kind) {
    /* MessageBeep est non bloquant, contrairement a Beep(). */
    if (kind == 2 || kind == 4) MessageBeep(MB_ICONHAND);
    else if (kind == 5) MessageBeep(MB_OK);
    else MessageBeep(MB_ICONASTERISK);
}

void input_poll(InputState *s) {
    memset(s, 0, sizeof(*s));
    s->p1_up    = (GetAsyncKeyState('Z') & 0x8000) != 0;
    s->p1_down  = (GetAsyncKeyState('S') & 0x8000) != 0;
    s->p1_left  = (GetAsyncKeyState('Q') & 0x8000) != 0;
    s->p1_right = (GetAsyncKeyState('D') & 0x8000) != 0;
    s->p1_bomb  = (GetAsyncKeyState('W') & 0x8000) != 0;

    s->p2_up    = (GetAsyncKeyState(VK_UP)    & 0x8000) != 0;
    s->p2_down  = (GetAsyncKeyState(VK_DOWN)  & 0x8000) != 0;
    s->p2_left  = (GetAsyncKeyState(VK_LEFT)  & 0x8000) != 0;
    s->p2_right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    s->p2_bomb  = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    s->quit     = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
    s->pause    = (GetAsyncKeyState('P') & 0x8000) != 0;

    /* On vide le buffer pour eviter un "beep" / des caracteres residuels. */
    while (_kbhit()) _getch();
}

void console_enable_colors(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

void console_set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

void console_reset_color(void) {
    console_set_color(COL_RESET);
}

void console_hide_cursor(int hide) {
    CONSOLE_CURSOR_INFO info;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleCursorInfo(h, &info)) {
        info.bVisible = hide ? FALSE : TRUE;
        SetConsoleCursorInfo(h, &info);
    }
}

void console_home(void) {
    COORD pos;
    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>

static struct termios orig_termios;
static int termios_saved = 0;

void input_init(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    termios_saved = 1;
    raw = orig_termios;
    raw.c_lflag &= ~((unsigned)(ICANON | ECHO));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    console_hide_cursor(1);
}

void input_restore(void) {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }
    console_hide_cursor(0);
    console_reset_color();
}

static int read_byte(void) {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) <= 0) return -1;
    return (int)c;
}

void input_flush(void) {
    unsigned char c;
    while (read(STDIN_FILENO, &c, 1) > 0) { }
}

void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void play_sfx(int kind) {
    (void)kind; /* pas de Beep portable ; les evenements restent geres */
}

void input_poll(InputState *s) {
    memset(s, 0, sizeof(*s));
    for (;;) {
        int c = read_byte();
        if (c < 0) break;
        if (c == 27) {
            int a = read_byte();
            int b = read_byte();
            if (a == '[' ) {
                if (b == 'A') s->p2_up = 1;
                else if (b == 'B') s->p2_down = 1;
                else if (b == 'C') s->p2_right = 1;
                else if (b == 'D') s->p2_left = 1;
            } else {
                s->quit = 1;
            }
            continue;
        }
        if (c == 'z' || c == 'Z') s->p1_up = 1;
        else if (c == 's' || c == 'S') s->p1_down = 1;
        else if (c == 'q' || c == 'Q') s->p1_left = 1;
        else if (c == 'd' || c == 'D') s->p1_right = 1;
        else if (c == 'w' || c == 'W') s->p1_bomb = 1;
        else if (c == ' ') s->p2_bomb = 1;
        else if (c == 'p' || c == 'P') s->pause = 1;
    }
}

void console_enable_colors(void) { }

void console_set_color(int color) {
    int code = 37;
    switch (color) {
        case COL_WALL:  code = 90; break;
        case COL_OBS:   code = 33; break;
        case COL_P1:    code = 32; break;
        case COL_P2:    code = 36; break;
        case COL_BOMB:  code = 31; break;
        case COL_BOOM:  code = 91; break;
        case COL_POWER: code = 35; break;
        case COL_TITLE: code = 33; break;
        default:        code = 37; break;
    }
    printf("\033[%dm", code);
}

void console_reset_color(void) {
    printf("\033[0m");
}

void console_hide_cursor(int hide) {
    printf(hide ? "\033[?25l" : "\033[?25h");
    fflush(stdout);
}

void console_home(void) {
    printf("\033[H");
}

#endif
