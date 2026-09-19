#include <string>

#include "document.h"
#include "fileio.h"
#include "input.h"
#include "screen.h"
#include "view.h"

int main(int argc, char *argv[])
{
    Document doc;
    std::string name = "default.txt";

    if(argc != 1)
    {
        name = get_filename(argv[1]);

        if(!load_file(argv[1], doc.lines))
            return 1;
    }

    screen_init();

    terminal_resize();      //刷新视口防止缓冲区影响

    Layout lay = screen_layout();
    View view;

    draw_text_area(doc, view, lay);
    draw_status(name, doc.row, doc.col, lay);
    screen_move_cursor(doc, view);
    screen_refresh();

    while(true)
    {
        char ch = 0;
        bool view_moved = false;

        switch(read_action(ch))
        {
        case Action::Quit:
            screen_close();
            return 0;

        case Action::Resize:
            terminal_resize();
            lay = screen_layout();
            view.clamp(doc, lay);           //行数变了，视口可能落到文档末尾之外
            view_moved = true;              //视口移动，整屏重画
            break;

        case Action::Up:        doc.move_up();       break;
        case Action::Down:      doc.move_down();     break;
        case Action::Left:      doc.move_left();     break;
        case Action::Right:     doc.move_right();    break;
        case Action::Backspace: doc.erase_before();  break;
        case Action::Enter:     doc.split_line();    break;
        case Action::Char:      doc.insert_char(ch); break;

        case Action::None:
            doc.edit = Edited{};              //清理Edited信息
            break;
        }

        if(view.ensure_visible(doc, lay))
            view_moved = true;

        if(view_moved)
            draw_text_area(doc, view, lay);
        else
            draw_text_edited(doc, view, lay);

        draw_status(name, doc.row, doc.col, lay);
        screen_move_cursor(doc, view);
        screen_refresh();
    }
}