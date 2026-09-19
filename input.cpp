#include <ctype.h>
#include <curses.h>

#include "input.h"

Action read_action(char &ch)
{
    int key = getch();

    ch = (char)key;

    switch(key)
    {
    case 0x11:          return Action::Quit;        //Ctrl-Q
    case KEY_RESIZE:    return Action::Resize;
    case KEY_UP:        return Action::Up;
    case KEY_DOWN:      return Action::Down;
    case KEY_LEFT:      return Action::Left;
    case KEY_RIGHT:     return Action::Right;
    case 0x08:          return Action::Backspace;
    case '\n':          return Action::Enter;
    }

    if(key >= 0 && key < 256 && isprint((unsigned char)key))
        return Action::Char;

    return Action::None;
}
