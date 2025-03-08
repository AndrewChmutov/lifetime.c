#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

// clang -o lifetime lifetime.c -lm -lcurses
// ./lifetime

#define PRECISION 10

#define YEAR 1900
#define MONTH 0
#define DAY 0
#define HOURS 0
#define MINUTES 0
#define SECONDS 0
#define TIMEZONE 0

#define SLEEP_MICROSECS 10000

#define STRINGIFY(x) #x
#define STRINGIFY_MACRO(x) STRINGIFY(x)
#define PRECISION_STR STRINGIFY_MACRO(PRECISION)

struct tm g_birth = {
  .tm_sec = SECONDS,
  .tm_min = MINUTES,
  .tm_hour = HOURS,
  .tm_mday = DAY,
  .tm_mon = MONTH,
  .tm_year = YEAR - 1900,
  .tm_gmtoff = TIMEZONE * 60 * 60,
};

double getAge(time_t *birth)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    double seconds = now.tv_sec + (double)now.tv_nsec / 1000000000 - *birth;
    return seconds / 60 / 60 / 24 / 365;
}

void drawBox(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void refreshOnResize(int *oldRow, int *oldCol, int newRow, int newCol)
{
    if (newRow != *oldRow || newCol != *oldCol) {
        *oldRow = newRow;
        *oldCol = newCol;
        clear();
    }
}

void center(int *centerY, int *centerX, int row, int col)
{
    (*centerY = row / 2, *centerX = col / 2);
}

void printBoxedAge(double years, int row, int col)
{
    int width = floor(log10(years)) + 1 + PRECISION;
    mvprintw(row, col - width / 2, "%."PRECISION_STR"f", years);
    drawBox(row - 1, col - width / 2 - 1, row + 1, col + 1 + ceil((double)width / 2));
}

void finish(int sig)
{
    endwin();
    exit(0);
}

int main()
{
    signal(SIGINT, finish);

    time_t birth = mktime(&g_birth);
    double age;

    int oldRow, oldCol;
    int row, col;
    int centerY, centerX;

    initscr();
    noecho();
    curs_set(0);
    getmaxyx(stdscr, oldRow, oldCol);

    while (1) {
        getmaxyx(stdscr, row, col);
        refreshOnResize(&oldRow, &oldCol, row, col);
        center(&centerY, &centerX, row, col);

        printBoxedAge(getAge(&birth), centerY, centerX);

        refresh();
        usleep(SLEEP_MICROSECS);
    }

    return 0;
}
