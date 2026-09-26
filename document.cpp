#include "document.h"

void Document::load(std::vector<std::string> loaded)
{
    lines = loaded;
    row = 0;
    col = 0;
    edit = Edited{};
}

void Document::move_to(int row, int col)
{
    this->row = row;
    this->col = col;

    if(row > (int)lines.size() - 1)
        this->row = (int)lines.size() - 1;
    if(col > (int)lines[this->row].size())
        this->col = (int)lines[row].size();
}

void Document::move_up()
{
    if(row > 0)
    {
        --row;

        if(col > (int)lines[row].size())
            col = (int)lines[row].size();
    }

    edit = Edited{};              //只是移动，屏幕不用重画
}

void Document::move_down()
{
    if(row + 1 < (int)lines.size())
    {
        ++row;

        if(col > (int)lines[row].size())
            col = (int)lines[row].size();
    }

    edit = Edited{};
}

void Document::move_left()
{
    if(col > 0)
        --col;
    else if(row > 0)
    {
        --row;
        col = (int)lines[row].size();
    }

    edit = Edited{};
}

void Document::move_right()
{
    if(col < (int)lines[row].size())
        ++col;
    else if(row + 1 < (int)lines.size())
    {
        ++row;
        col = 0;
    }

    edit = Edited{};
}

void Document::insert_char(char ch)
{
    int at = col;

    lines[row].insert(col, 1, ch);
    ++col;

    edit = Edited{row, at, false};
}

void Document::split_line()
{
    lines.insert(lines.begin() + row + 1, lines[row].substr(col));
    lines[row].erase(col);
    ++row;
    col = 0;

    edit = Edited{row - 1, -1, false};
}

void Document::erase_before()
{
    if(col > 0)
    {
        int at = col - 1;

        lines[row].erase(at, 1);
        --col;

        edit = Edited{row, at, true};
    }
    else if(row > 0)
    {
        col = (int)lines[row - 1].size();
        lines[row - 1] += lines[row];
        lines.erase(lines.begin() + row);
        --row;

        edit = Edited{row, -1, false};
    }
    else
        edit = Edited{};
}

void Document::erase_front()
{
    if(col < (int)lines[row].size())
    {
        lines[row].erase(col, 1);
        edit = Edited{row, col, true};
    }
    else if(row < (int)lines.size() - 1)
    {
        lines[row] += lines[row + 1];
        lines.erase(lines.begin() + row + 1);
        edit = Edited{row, -1, false};
    }
    else
        edit = Edited{};
}
