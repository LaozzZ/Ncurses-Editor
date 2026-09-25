#include "quickaction.h"

#include <vector>


std::string Search::search_msg()
{
    return "Search: " + query + position;
}

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
            search_results.insert(search_results.begin(), {(int)search_results.size(), (int)search_results.size()});
    }
}

void Search::search_turn_page(Document &doc)
{
    if(search_results[0][0] != -1)
    {
        int &pos = search_results[0][0];
        int &length = search_results[0][1];

        pos = pos % length + 1;

        position = " " + std::to_string(pos) + " / " + std::to_string(length);
        doc.move_to(search_results[pos][0], search_results[pos][1]);
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

void Search::search_clear()
{
    search_results.clear();
    query.clear();
    position.clear();
}


std::string Replace::replace_msg()
{
    if(replace_stage != 0)
        return se.search_msg() + " Replace to: " + target;
    return se.search_msg();
}

void Replace::replace_insert_char(char ch)
{
    if(replace_stage == 1)
        target += ch;
    else if(replace_stage == 0)
        se.query += ch;
}

void Replace::replace_erase()
{
    if(replace_stage == 1 && !target.empty())
        target.pop_back();
    else if(replace_stage == 0 && !se.query.empty())
        se.query.pop_back();
}

void Replace::replace_this(Document &doc)
{
    if(se.search_results[0][0] == -1 || !replace_map[se.search_results[0][0]])
    {
        se.search_turn_page(doc);
        return;
    }

    doc.lines[doc.row].replace(doc.col, (int)se.query.size(), target);
    doc.edit = {doc.row, doc.col, true};

    replace_map[se.search_results[0][0]] = false;
    se.search_turn_page(doc);
}

void Replace::replace_all(Document &doc)
{
    for(int i = 0;i < se.search_results[0][1];i++)
        replace_this(doc);
}

void Replace::replace_clear()
{
    se.search_clear();
    target.clear();
    replace_stage = 0;
}