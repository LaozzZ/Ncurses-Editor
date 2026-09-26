#include "view.h"

bool View::ensure_visible(const Document &doc, Layout lay)
{
    int prev_top_row = top_row;
    int prev_left_col = left_col;

    if(doc.row < top_row)
        top_row = doc.row;
    else if(doc.row >= top_row + lay.rows)
        top_row = doc.row - lay.rows + 1;

    if(doc.col < left_col)
        left_col = doc.col;
    else if(doc.col >= left_col + lay.cols)
        left_col = doc.col - lay.cols + 1;

    if(top_row < 0)
        top_row = 0;

    if(left_col < 0)
        left_col = 0;

    return prev_top_row != top_row || prev_left_col != left_col;
}

void View::clamp(const Document &doc, Layout lay)
{
    if(top_row > doc.line_count() - lay.rows)
        top_row = doc.line_count() - lay.rows;

    if(top_row < 0)
        top_row = 0;
}
