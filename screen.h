#pragma once

#include <string>

#include "document.h"
#include "view.h"

struct StatusLine
{
    std::string name = "default.txt";
    bool modified = false;
    std::string message;
    int msg_color = 0;
    int row = 1;
    int col = 1;

    void status_refresh(const Document &doc, bool search, bool replace);
};

Layout screen_layout();

void screen_init();
void screen_close();
void screen_refresh();

void screen_move_cursor(const Document &doc, const View &view);

void draw_text_area(const Document &doc, const View &view, Layout lay);
void draw_text_edited(const Document &doc, const View &view, Layout lay);
void draw_status(const StatusLine &state, Layout lay);

void terminal_resize();
