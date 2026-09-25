#pragma once

#include "document.h"

#include <string>

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

    void replace_this(Document &doc);

    void replace_all(Document &doc);

    void replace_clear();
};