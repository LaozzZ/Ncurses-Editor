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

History Replace::replace_this(Document &doc)
{
    if(se.search_results[0][0] == -1 || !replace_map[se.search_results[0][0]])
    {
        se.search_turn_page(doc);
        return {};
    }

    History res = {Type::Replace, {{doc.row, doc.col}}, {doc.line(doc.row)}};
    doc.lines[doc.row].replace(doc.col, (int)se.query.size(), target);
    doc.edit = {doc.row, doc.col, true};

    replace_map[se.search_results[0][0]] = false;
    se.search_turn_page(doc);

    return res;
}

History Replace::replace_all(Document &doc)
{
    History res = {Type::Replace, {}, {}};

    for(int i = 0;i < se.search_results[0][1];i++)
    {
        History temp = replace_this(doc);
        if(!temp.poses.empty() && (res.poses.empty() || temp.poses[0][0] != res.poses.back()[0]))
        {
            res.poses.push_back(temp.poses.back());
            res.prev_lines.push_back(temp.prev_lines.back());
        }
    }

    return res;
}

void Replace::replace_clear()
{
    se.search_clear();
    target.clear();
    replace_stage = 0;
}


void Historys::withdraw(Document &doc, bool &view_moved)
{
    if(history_dq.empty())
        return;

    History prev = history_dq.back();
    history_dq.pop_back();

    switch (prev.type)
    {
    case Type::Replace:
        for(int i = 0;i < (int)prev.poses.size();i++)
        {
            doc.lines[prev.poses[i][0]] = prev.prev_lines[i];
            doc.move_to(prev.poses[i][0], prev.poses[i][1]);
        }
        doc.edit = {doc.row, doc.col, true};
        view_moved = true;
        break;
    
    case Type::Enter:
        doc.move_to(prev.poses[0][0], prev.poses[0][1]);
        doc.erase_before();
        break;

    case Type::Delete_line:
        doc.move_to(prev.poses[0][0], prev.poses[0][1]);
        doc.split_line();
        break;

    case Type::Delete_before:
        doc.lines[prev.poses[0][0]] = prev.prev_lines[0];
        doc.move_to(prev.poses[0][0], prev.poses[0][1]);
        doc.edit = {doc.row, 0, true};
        break;

    case Type::Delete_front:
        doc.lines[prev.poses[0][0]] = prev.prev_lines[0];
        doc.move_to(prev.poses[0][0], prev.poses[0][1]);
        doc.edit = {doc.row, 0, true};
        break;

    case Type::Str:
        doc.lines[prev.poses[0][0]] = prev.prev_lines[0];
        doc.move_to(prev.poses[0][0], prev.poses[0][1]);
        doc.edit = {doc.row, 0, true};
        break;

    default:
        break;
    }
}

void Historys::push(const History &history)
{
    if(history.poses.empty())
        return;
    if(!history_dq.empty() && is_ongoing && history.type == Type::Str && history_dq.back().type == Type::Str &&
       history.poses[0][0] == history_dq.back().poses[0][0] && history.poses[0][1] == history_dq.back().poses[0][1] + 1)
    {
        history_dq.back().poses[0][1]++;
        return;
    }
    if(!history_dq.empty() && is_ongoing && history.type == Type::Delete_before && history_dq.back().type == Type::Delete_before &&
       history.poses[0][0] == history_dq.back().poses[0][0] && history.poses[0][1] == history_dq.back().poses[0][1] - 1)
    {
        history_dq.back().poses[0][1]--;
        return;
    }
    if(!history_dq.empty() && is_ongoing && history.type == Type::Delete_front && history_dq.back().type == Type::Delete_front &&
       history.poses[0][0] == history_dq.back().poses[0][0] && history.poses[0][1] == history_dq.back().poses[0][1])
        return;

    history_dq.push_back(history);
    if(history_dq.size() > 20)
        history_dq.pop_front();
}

void Historys::erase_before(Document &doc)
{
    History history;
    if(doc.col == 0 && doc.row == 0)
        return;
    else if(doc.col != 0)
        history = {Type::Delete_before, {{doc.row, doc.col - 1}}, {doc.line(doc.row)}};
    else
        history = {Type::Delete_line, {{doc.row - 1, (int)doc.line(doc.row - 1).size()}}, {}};
    doc.erase_before();
    push(history);

    is_ongoing = true;
}

void Historys::erase_front(Document &doc)
{
    History history;
    if(doc.col == (int)doc.line(doc.row).size() && doc.row == doc.line_count() - 1)
        return;
    else if(doc.col != (int)doc.line(doc.row).size())
        history = {Type::Delete_front, {{doc.row, doc.col}}, {doc.line(doc.row)}};
    else
        history = {Type::Delete_line, {{doc.row, doc.col}}, {}};
    doc.erase_front();
    push(history);

    is_ongoing = true;
}

void Historys::split_line(Document &doc)
{
    History history = {Type::Enter, {{doc.row + 1, 0}}, {}};
    doc.split_line();
    push(history);
}

void Historys::insert_char(Document &doc, char ch)
{
    if(ch == ' ')
        is_ongoing = false;
    else
        is_ongoing = true;

    History history = {Type::Str, {{doc.row, doc.col}}, {doc.line(doc.row)}};
    doc.insert_char(ch);
    push(history);
}

void Historys::ongoing_check(Action action)
{
    if(action == Action::Up || action == Action::Down || action == Action::Left || action == Action::Right || action == Action::Enter)
        is_ongoing = false;
}
