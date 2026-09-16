#include <curses.h>
#include <ctype.h>
#include <string>
#include <vector>

static std::vector<std::string> lines(1);
static int prev_row = 0;           //上次输入后的总行数

//重画第 row 行 [from - end] 的部分
static void draw_row(int row, int from, bool clear_tail)
{
    if (row >= LINES || from >= COLS)
        return;

    wmove(stdscr, row, from);

    if (clear_tail)
        wclrtoeol(stdscr);

    waddnstr(stdscr, lines[row].c_str() + from, COLS - from);
}

//重画第 row 行至文档结尾的部分
static void draw_from(int row)
{
    int last_row = (int)lines.size();

    if (last_row > LINES)
        last_row = LINES;

    for (int i = row; i < last_row; i++)
    {
        wmove(stdscr, i, 0);
        wclrtoeol(stdscr);
        waddnstr(stdscr, lines[i].c_str(), COLS);
    }

    if (prev_row > last_row)           //行数减少,需清除尾部
    {
        wmove(stdscr, last_row, 0);
        wclrtoeol(stdscr);
    }

    prev_row = last_row;
}

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();

    int row = 0;
    int col = 0;

    draw_from(0);

    int ch;

    while ((ch = getch()) != 0x11)      //Ctrl-Q
    {
        int dirty_row = -1;         //需处理的 行/列
        int dirty_col = -1;
        bool clear_tail = false;        //是否要清除尾部

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

        case 0x08:      //退格
            if (col > 0)
            {
                dirty_row = row;
                dirty_col = col - 1;
                clear_tail = true;

                lines[row].erase(col - 1, 1);
                --col;
            }
            else if (row > 0)
            {
                col = (int)lines[row - 1].size();
                lines[row - 1] += lines[row];
                lines.erase(lines.begin() + row);
                --row;

                dirty_row = row;
            }
            break;

        case '\n':      //换行
            if ((int)lines.size() < LINES)      //屏幕已满则拦截
            {
                lines.insert(lines.begin() + row + 1, lines[row].substr(col));
                lines[row].erase(col);
                row++;
                col = 0;

                dirty_row = row - 1;
            }
            break;

        default:
            if (isprint(ch) && (int)lines[row].size() < COLS - 1)   //本行已满则拦截
            {
                dirty_row = row;
                dirty_col = col;

                lines[row].insert(col, 1, (char)ch);
                col++;
            }
            break;
        }

        if (dirty_row >= 0)
        {
            if (dirty_col >= 0)
                draw_row(dirty_row, dirty_col, clear_tail);
            else
                draw_from(dirty_row);
        }

        wmove(stdscr, row, col < COLS ? col : COLS - 1);
        refresh();
    }

    endwin();
    return 0;
}
