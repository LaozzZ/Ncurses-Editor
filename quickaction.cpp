#include "quickaction.h"


void Search::search_init(const Document &doc)
{
    if(search_results.empty())
    {
        for(int i = 0;i < doc.line_count();i++)
        {
            if(query.empty())
                break;

            size_t col = 0;
            while((col = doc.line(i).find(query, col)) != std::string::npos)
            {
                search_results.push_back({i, (int)col});
                col += query.size();
            }
        }

        if(search_results.empty())
            search_results.push_back({-1, -1});
        else
            search_results.insert(search_results.begin(), {1, (int)search_results.size()});
    }
}

void Search::search_turn_page(Document &doc)
{
    if(search_results[0][0] != -1)      //后续按下Enter时的 "下一个" 操作
    {
        int &pos = search_results[0][0];
        int &length = search_results[0][1];
        position = " " + std::to_string(pos) + " / " + std::to_string(length);
        doc.move_to(search_results[pos][0], search_results[pos][1]);
        pos = pos % length + 1;
    }
    else
        position = " Failed";
}

void Search::search_insert_char(char ch)
{
    query += ch;
    search_results.clear();
    position.clear();
}

void Search::search_erase()
{
    if(!query.empty())
    {
        query.pop_back();
        search_results.clear();
    }
    position.clear();
}