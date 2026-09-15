#include <curses.h>
#include <ctype.h>

#include <string>
#include <vector>

static void draw(const std::vector<std::string> &lines, int row, int col)
{
    erase();

    for (int i = 0; i < (int)lines.size(); ++i)
        mvprintw(i, 0, "%s", lines[i].c_str());

    move(row, col);
    refresh();
}

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();

    std::vector<std::string> lines(1);
    int row = 0;
    int col = 0;

    draw(lines, row, col);

    int ch;

    while ((ch = getch()) != 27)
    {
        bool edited = false;

        switch (ch)
        {
        case KEY_UP:
            if (row > 0)
            {
                --row;

                if (col > (int)lines[row].size())
                    col = (int)lines[row].size();
            }
            break;

        case KEY_DOWN:
            if (row + 1 < (int)lines.size())
            {
                ++row;

                if (col > (int)lines[row].size())
                    col = (int)lines[row].size();
            }
            break;

        case KEY_LEFT:
            if (col > 0)
                --col;
            else if (row > 0)
            {
                --row;
                col = (int)lines[row].size();
            }
            break;

        case KEY_RIGHT:
            if (col < (int)lines[row].size())
                ++col;
            else if (row + 1 < (int)lines.size())
            {
                ++row;
                col = 0;
            }
            break;

        case 0x08:
            if (col > 0)
            {
                lines[row].erase(col - 1, 1);
                --col;
                edited = true;
            }
            else if (row > 0)
            {
                col = (int)lines[row - 1].size();
                lines[row - 1] += lines[row];
                lines.erase(lines.begin() + row);
                --row;
                edited = true;
            }
            break;

        case '\r':
        case '\n':
            lines.insert(lines.begin() + row + 1, lines[row].substr(col));
            lines[row].erase(col);
            ++row;
            col = 0;
            edited = true;
            break;

        default:
            if (isprint(ch))
            {
                lines[row].insert(col, 1, (char)ch);
                ++col;
                edited = true;
            }
            break;
        }

        if (edited)
            draw(lines, row, col);
        else
        {
            move(row, col);
            refresh();
        }
    }

    endwin();
    return 0;
}
