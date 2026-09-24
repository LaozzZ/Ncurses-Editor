#pragma once

enum class Action
{
    None,           //不处理
    Search,
    Save,           //Ctrl-S
    Quit,           //Ctrl-Q
    Esc,
    Resize,         //窗口尺寸改变
    Up,
    Down,
    Left,
    Right,
    Backspace,
    Del,
    Enter,
    Char
};

Action read_action(char &ch);