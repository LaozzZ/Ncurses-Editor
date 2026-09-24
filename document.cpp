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


void Document::search(const std::string &target, std::string &position, std::vector<std::vector<int>> &search_results)
{
    if(search_results.empty())          //第一次按下Enter时进行的查找操作
    {
        for(int i = 0;i < (int)lines.size();i++)
        {
            if(target.empty())
                break;

            size_t col = 0;
            while((col = lines[i].find(target, col)) != std::string::npos)
            {
                search_results.push_back({i, (int)col});
                col += target.size();
            }
        }

        if(search_results.empty())
            search_results.push_back({-1, -1});
        else
            search_results.insert(search_results.begin(), {1, (int)search_results.size()});
    }

    if(search_results[0][0] != -1)      //后续按下Enter时的 "下一个" 操作
    {
        int &pos = search_results[0][0];
        int &length = search_results[0][1];
        position = " " + std::to_string(pos) + " / " + std::to_string(length);
        row = search_results[pos][0];
        col = search_results[pos][1];
        pos = pos % length + 1;
    }
    else
        position = " Failed";
}