#pragma once

#include <string>
#include <vector>

//存放脏文本信息的结构体
struct Edited
{
    int dirty_row = -1;
    int dirty_col = -1;
    bool clear_tail = false;
};

//存放文本、光标信息、编辑信息的结构体
struct Document
{
    std::vector<std::string> lines{std::string()};
    int row = 0;                //光标所在行
    int col = 0;                //光标所在列
    Edited edit;                  //受编辑的影响范围

    int line_count() const
    {
        return (int)lines.size();
    }

    const std::string &line(int r) const
    {
        return lines[r];
    }

    void load(std::vector<std::string> loaded);

    void move_up();
    void move_down();
    void move_left();
    void move_right();

    void insert_char(char ch);
    void split_line();          //Enter
    void erase_before();        //退格
    void erase_front();         //Delete
};
