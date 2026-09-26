#include <curses.h>

#include "screen.h"

//重画第 row 行从 from 列到行尾的部分       row / from 是实际位置而非相对位置
static void draw_row(const Document &doc, const View &view, Layout lay, int row, int from, bool clear_tail)
{
    int y = row - view.top_row;
    int x = from - view.left_col;

    if(y < 0 || y >= lay.rows || x < 0 || x >= lay.cols)
        return;

    wmove(stdscr, y, x);

    if(clear_tail)
        wclrtoeol(stdscr);

    waddnstr(stdscr, doc.line(row).c_str() + from, lay.cols - x);
}

//重画第 row 行到书写区最底部的部分         row 是实际位置而非相对位置
static void draw_from(const Document &doc, const View &view, Layout lay, int row)
{
    int y = row - view.top_row;

    if(y < 0)
        y = 0;

    for(int i = y; i < lay.rows; i++)
    {
        int text_row = view.top_row + i;

        wmove(stdscr, i, 0);
        wclrtoeol(stdscr);

        if(text_row < doc.line_count() && view.left_col < (int)doc.line(text_row).size())
            waddnstr(stdscr, doc.line(text_row).c_str() + view.left_col, lay.cols);
    }
}

//刷新状态栏参数
void StatusLine::status_refresh(const Document &doc, bool search, bool replace)
{
    row = doc.row + 1;
    col = doc.col + 1;
    modified = doc.edit.dirty_row >= 0 || modified;
    if(message == "Saved" || search || replace)
        msg_color = 1;
    else if(message == "Unsaved changes, press Ctrl-Q again to quit")
        msg_color = 2;
    else
        msg_color = 3;
}

//返回书写区尺寸
Layout screen_layout()
{
    return Layout{LINES - 1, COLS};
}

//初始化curses
void screen_init()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
}

//关闭curses
void screen_close()
{
    endwin();
}

//刷新curses
void screen_refresh()
{
    refresh();
}

//移动光标
void screen_move_cursor(const Document &doc, const View &view)
{
    wmove(stdscr, doc.row - view.top_row, doc.col - view.left_col);
}

//重画整个书写区
void draw_text_area(const Document &doc, const View &view, Layout lay)
{
    draw_from(doc, view, lay, view.top_row);
}

//重画受编辑的区域
void draw_text_edited(const Document &doc, const View &view, Layout lay)
{
    if(doc.edit.dirty_row < 0)
        return;

    if(doc.edit.dirty_col >= 0)
        draw_row(doc, view, lay, doc.edit.dirty_row, doc.edit.dirty_col, doc.edit.clear_tail);
    else
        draw_from(doc, view, lay, doc.edit.dirty_row);
}

//画状态栏
void draw_status(const StatusLine &state, Layout lay)
{
    std::string name = state.name;

    if(state.modified)
        name.insert(name.begin(), '*');

    std::string texts[3] = {name,
                            std::to_string(state.row) + " : " + std::to_string(state.col),
                            state.message};
    int pairs[3] = {state.modified ? 2 : 1, 0, state.msg_color};

    int x = 0;
    wmove(stdscr, lay.rows, 0);

    for(int i = 0; i < 3; i++)
    {
        if(texts[i].empty())
            continue;

        int sep = x > 0 ? 3 : 0;

        if(x + sep >= lay.cols)
            break;

        if(sep)
        {
            wattrset(stdscr, A_NORMAL);
            waddnstr(stdscr, " | ", sep);
            x += sep;
        }

        int n = (int)texts[i].size();

        if(n > lay.cols - x)
            n = lay.cols - x;               //超出部分截断

        wattrset(stdscr, COLOR_PAIR(pairs[i]));
        waddnstr(stdscr, texts[i].c_str(), n);
        x += n;
    }

    wattrset(stdscr, A_NORMAL);
    wclrtoeol(stdscr);
}

//终端尺寸改变时的处理
void terminal_resize()
{
    if(resize_term(0, 0) == ERR)
        resize_term(2, COLS);

    clear();
}
