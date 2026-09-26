#pragma once

#include "document.h"
#include "input.h"

#include <string>
#include <deque>

enum class Type {Str, Delete_before, Delete_front, Delete_line, Enter, Replace};

struct History
{
    Type type;
    std::vector<std::vector<int>> poses;
    std::vector<std::string> prev_lines;
};

struct Historys
{
    std::deque<History> history_dq;
    bool is_ongoing = false;

    void withdraw(Document &doc, bool &view_moved);
    void push(const History &history);

    void erase_before(Document &doc);
    void erase_front(Document &doc);
    void split_line(Document &doc);
    void insert_char(Document &doc, char ch);

    void ongoing_check(Action action);
};

struct Search
{
    std::vector<std::vector<int>> search_results;
    std::string query;
    std::string position;


    std::string search_msg();

    void search_init(const Document &doc);

    void search_turn_page(Document &doc);

    void search_insert_char(char ch);

    void search_erase();

    void search_clear();
};

struct Replace
{
    std::vector<bool> replace_map;
    int replace_stage = 0;
    std::string target;

    Search se;

    std::string replace_msg();

    void replace_insert_char(char ch);

    void replace_erase();

    History replace_this(Document &doc);

    History replace_all(Document &doc);

    void replace_clear();
};
