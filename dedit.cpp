#include <string>

#include "document.h"
#include "fileio.h"
#include "input.h"
#include "screen.h"
#include "view.h"
#include "quickaction.h"

int main(int argc, char *argv[])
{
    Document doc;
    StatusLine state;
    Search se;

    if(argc != 1)
    {
        state.name = get_filename(argv[1]);

        if(!load_file(argv[1], doc.lines))
            return 1;
    }

    screen_init();

    terminal_resize();      //刷新视口防止缓冲区影响

    Layout lay = screen_layout();       //view相关
    View view;

    draw_text_area(doc, view, lay);     //screen相关
    draw_status(state, lay);
    screen_move_cursor(doc, view);
    screen_refresh();

    bool quit_warning = false;          //Save相关

    bool search_mode = false;           //Search相关

    while(true)
    {
        char ch = 0;
        bool view_moved = false;
        doc.edit = Edited{};

        std::string error;
        state.message.clear();          //清除状态栏message

        Action action = read_action(ch);
        if(action != Action::Quit)
            quit_warning = false;

        if(search_mode)             //搜索模式
        {          
            if(action == Action::Char)
                se.search_insert_char(ch);
            else if(action == Action::Backspace)
                se.search_erase();
            else if(action == Action::Enter)
            {
                se.search_init(doc);
                se.search_turn_page(doc);
            }
            else if(action == Action::Resize)
            {
                terminal_resize();
                lay = screen_layout();
                view.clamp(doc, lay);
                view_moved = true;
            }
            
            state.message = se.search_msg();
                        
            if(action == Action::Esc)
            {
                search_mode = false;
                state.message.clear();
                se.search_refresh();
            }            
        }
        else switch(action)
        {
            case Action::Search:
                state.message = "Search: ";
                search_mode = true;
                break;

            case Action::Save:
                if(!save_file(argv[1] ? argv[1] : "default.txt", doc.lines, error))
                    state.message = "Save failed: " + error;
                else
                {
                    state.message = "Saved";
                    state.modified = false;
                }
                break;

            case Action::Quit:
                if(quit_warning == false && state.modified)
                {
                    state.message = "Unsaved changes, press Ctrl-Q again to quit";
                    quit_warning = true;
                    break;
                }

                screen_close();
                return 0;

            case Action::Resize:
                terminal_resize();
                lay = screen_layout();
                view.clamp(doc, lay);
                view_moved = true;              //视口移动，整屏重画
                break;

            case Action::Up:        doc.move_up();       break;
            case Action::Down:      doc.move_down();     break;
            case Action::Left:      doc.move_left();     break;
            case Action::Right:     doc.move_right();    break;
            case Action::Backspace: doc.erase_before();  break;
            case Action::Del:       doc.erase_front();   break;
            case Action::Enter:     doc.split_line();    break;
            case Action::Char:      doc.insert_char(ch); break;

            case Action::None:      break;
            case Action::Esc:       break;
        }

        if(view.ensure_visible(doc, lay))
            view_moved = true;

        if(view_moved)
            draw_text_area(doc, view, lay);
        else
            draw_text_edited(doc, view, lay);
        
        state.status_refresh(doc, search_mode);


        draw_status(state, lay);
        screen_move_cursor(doc, view);      //光标移位
        screen_refresh();
    }
}
