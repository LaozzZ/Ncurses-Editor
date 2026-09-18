#include <curses.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>

static std::vector<std::string> lines(1);
static int top_row = 0;            //屏幕第 0 行对应文本的第几行
static int left_col = 0;           //屏幕第 0 列对应文本的第几列
static std::string file_name = "default.txt";       //状态栏文件名

//从路径里取文件名，\ 和 / 都认
static std::string get_name(const char *path)
{
    std::string full(path);
    int cut = full.find_last_of("\\/");

    if(cut != std::string::npos)
        full.erase(0, cut + 1);

    return full;
}

//读入文件
static bool load_file(const char *path)
{
    std::ifstream in(path, std::ios::binary);

    if(!in)
    {
        if(errno == ENOENT)
            fprintf(stderr, "错误：文件不存在：%s\n", path);
        else
            fprintf(stderr, "错误：无法读取文件：%s(%s)\n", path, strerror(errno));

        return false;
    }

    std::string line;

    lines.clear();

    while(std::getline(in, line))      //按行读取
    {
        if(!line.empty() && line.back() == '\r')
            line.pop_back();            //去掉行结尾'\r'

        lines.push_back(line);
    }

    if(lines.empty())                  //空文件处理
        lines.push_back("");

    return true;
}

//保证光标在屏幕内
static bool ensure_visible(int row, int col)
{
    int prev_top_row = top_row;
    int prev_left_col = left_col;

    if(row < top_row)
        top_row = row;
    else if(row >= top_row + LINES - 1)
        top_row = row - (LINES - 1) + 1;

    if(col < left_col)
        left_col = col;
    else if(col >= left_col + COLS)
        left_col = col - COLS + 1;

    if(top_row < 0)
        top_row = 0;

    if(left_col < 0)
        left_col = 0;

    return prev_top_row != top_row || prev_left_col != left_col;        //检测视口是否改变
}

//重画文本第 row 行从 from 列到行尾的部分
static void draw_row(int row, int from, bool clear_tail)
{
    int y = row - top_row;
    int x = from - left_col;

    if(y < 0 || y >= LINES - 1 || x < 0 || x >= COLS)
        return;

    wmove(stdscr, y, x);

    if(clear_tail)
        wclrtoeol(stdscr);

    waddnstr(stdscr, lines[row].c_str() + from, COLS - x - 1);
}

//重画文本第 row 行到屏幕底部的部分
static void draw_from(int row)
{
    int y = row - top_row;

    if(y < 0)
        y = 0;

    for(int i = y; i < LINES - 1; i++)
    {
        int text_row = top_row + i;

        wmove(stdscr, i, 0);
        wclrtoeol(stdscr);

        if(text_row < (int)lines.size() && left_col < (int)lines[text_row].size())
            waddnstr(stdscr, lines[text_row].c_str() + left_col, COLS - 1);
    }
}

static void draw_status(int row, int col)
{
    int width = COLS - 1;
    int name_len = (int)file_name.size();

    if(name_len > width)
        name_len = width;

    std::string text = file_name + " | " + std::to_string(row + 1) + " : " + std::to_string(col + 1);

    text.resize(width, ' ');        //超出则截断，不足则补足

    wmove(stdscr, LINES - 1, 0);

    wattrset(stdscr, COLOR_PAIR(1));
    waddnstr(stdscr, file_name.c_str(), name_len);
    wattrset(stdscr, A_NORMAL);
    waddnstr(stdscr, text.c_str() + name_len, width - name_len);
}

int main(int argc, char *argv[])
{
    if(argc != 1)
    {
        file_name = get_name(argv[1]);

        if(!load_file(argv[1]))
            return 1;
    }

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    int row = 0;
    int col = 0;

    draw_from(0);
    draw_status(row, col);

    wmove(stdscr, 0, 0);
    refresh();

    int ch;

    while((ch = getch()) != 0x11)      //Ctrl-Q
    {
        int dirty_row = -1;         //需处理的 行/列
        int dirty_col = -1;
        bool clear_tail = false;        //是否要清除尾部
        bool redraw = false;            //是否整屏重画

        switch(ch)
        {
        case KEY_UP:
            if(row > 0)
            {
                --row;

                if(col > (int)lines[row].size())
                    col = (int)lines[row].size();
            }
            break;

        case KEY_DOWN:
            if(row + 1 < (int)lines.size())
            {
                ++row;

                if(col > (int)lines[row].size())
                    col = (int)lines[row].size();
            }
            break;

        case KEY_LEFT:
            if(col > 0)
                --col;
            else if(row > 0)
            {
                --row;
                col = (int)lines[row].size();
            }
            break;

        case KEY_RIGHT:
            if(col < (int)lines[row].size())
                ++col;
            else if(row + 1 < (int)lines.size())
            {
                ++row;
                col = 0;
            }
            break;

        case 0x08:      //退格
            if(col > 0)
            {
                dirty_row = row;
                dirty_col = col - 1;
                clear_tail = true;

                lines[row].erase(col - 1, 1);
                --col;
            }
            else if(row > 0)
            {
                col = (int)lines[row - 1].size();
                lines[row - 1] += lines[row];
                lines.erase(lines.begin() + row);
                --row;

                dirty_row = row;
            }
            break;

        case '\n':      //换行
            lines.insert(lines.begin() + row + 1, lines[row].substr(col));
            lines[row].erase(col);
            row++;
            col = 0;

            dirty_row = row - 1;
            break;

        case KEY_RESIZE:        //窗口大小改变
            if(resize_term(0, 0) == ERR)        //窗口小于(2, 2)时重建
                resize_term(2, COLS);

            clear();

            //视口行数改变，处理极端情况视口位置
            if(top_row > (int)lines.size() - (LINES - 1))
                top_row = (int)lines.size() - (LINES - 1);

            if(top_row < 0)
                top_row = 0;

            redraw = true;
            break;

        default:
            if(isprint(ch))
            {
                dirty_row = row;
                dirty_col = col;

                lines[row].insert(col, 1, (char)ch);
                col++;
            }
            break;
        }

        if(ensure_visible(row, col) || redraw)      //这里会执行ensure_visible(row, col)判断
            draw_from(top_row);         //视口滚动了，重画
        else if(dirty_row >= 0 && dirty_col >= 0)
            draw_row(dirty_row, dirty_col, clear_tail);
        else if(dirty_row >= 0)
            draw_from(dirty_row);

        draw_status(row, col);
        wmove(stdscr, row - top_row, col - left_col);
        refresh();
    }

    endwin();
    return 0;
}
