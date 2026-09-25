#pragma once

#include "document.h"

#include <string>

struct Search
{
    std::vector<std::vector<int>> search_results;
    std::string query;
    std::string position;

    void search_refresh()
    {
        search_results.clear();
        query.clear();
        position.clear();
    }

    std::string search_msg()
    {
        return "Search: " + query + position;
    }

    void search_init(const Document &doc);

    void search_turn_page(Document &doc);

    void search_insert_char(char ch);

    void search_erase();
};