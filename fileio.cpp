#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "fileio.h"

bool load_file(const char *path, std::vector<std::string> &lines)
{
    std::ifstream in(path, std::ios::binary);

    if(!in)
    {
        if(errno == ENOENT)
            fprintf(stderr, "错误：文件不存在：%s\n", path);
        else
            fprintf(stderr, "错误：无法读取文件：%s(%s)\n", path, strerror(errno));

        return false;
    }

    std::string line;

    lines.clear();

    while(std::getline(in, line))       //按行读取
    {
        if(!line.empty() && line.back() == '\r')
            line.pop_back();            //去掉行结尾'\r'

        lines.push_back(line);
    }

    if(lines.empty())                   //空文件处理
        lines.push_back("");

    return true;
}

std::string get_filename(const char *path)
{
    std::string full(path);
    size_t cut = full.find_last_of("\\/");

    if(cut != std::string::npos)
        full = full.substr(cut + 1);

    return full;
}
