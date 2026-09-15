#include <curses.h>
#include <ctype.h>

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();

    int ch;

    while ((ch = getch()) != 27)
    {
        int y, x;
        getyx(stdscr, y, x);

        switch (ch)
        {
        case KEY_UP:
            move(y - 1, x);
            break;

        case KEY_DOWN:
            move(y + 1, x);
            break;

        case KEY_LEFT:
            move(y, x - 1);
            break;

        case KEY_RIGHT:
            move(y, x + 1);
            break;

        default:
            if (isprint(ch))
                addch(ch);
            break;
        }

        refresh();
    }

    endwin();
    return 0;
}
