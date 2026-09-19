#pragma once

#include <string>

#include "document.h"
#include "view.h"

Layout screen_layout();

void screen_init();
void screen_close();
void screen_refresh();

void screen_move_cursor(const Document &doc, const View &view);

void draw_text_area(const Document &doc, const View &view, Layout lay);
void draw_text_edited(const Document &doc, const View &view, Layout lay);
void draw_status(const std::string &name, int row, int col, Layout lay);

void terminal_resize();
