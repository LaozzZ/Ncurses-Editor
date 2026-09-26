#pragma once

#include "document.h"

//显示文本区域的长宽,rows = LINES - 1, cols = COLS
struct Layout
{
    int rows;
    int cols;
};

//视口的相对位置
struct View
{
    int top_row = 0;        //视口第一行是文本的第几行
    int left_col = 0;       //视口第一列是文本的第几列

    //检测视口是否移动，保证光标可见
    bool ensure_visible(const Document &doc, Layout lay);

    //终端尺寸变化时，处理可能导致光标移出视口的极端情况
    void clamp(const Document &doc, Layout lay);
};
