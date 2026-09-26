#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "fileio.h"

namespace fs = std::filesystem;

bool load_file(const char *path, std::vector<std::string> &lines)
{
    std::ifstream in(path, std::ios::binary);

    if(!in)
    {
        if(errno == ENOENT)
            std::cerr << "错误：文件不存在：" << path << "\n";
        else
            std::cerr << "错误：无法读取文件：" << path << "(" << std::strerror(errno) << ")\n";

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

    return !in.bad();
}

bool save_file(const char *path, const std::vector<std::string> &lines, std::string& error)
{
    std::string full(path);
    std::string temp_path;
    size_t cut = full.find_last_of("\\/");

    if(cut != std::string::npos)
        temp_path += full.substr(0, cut + 1);
    temp_path += ".dedit_tmp";
    std::error_code ec;
    
    std::ofstream out(temp_path, std::ios::binary);

    if(!out)
    {
        error = "Cannot create temp file: " + temp_path + "(" + std::strerror(errno) + ")";

        return false;
    }

    for(const std::string& line:lines)
    {
        out.write(line.data(), (std::streamsize)line.size());
        out.put('\n');
    }

    out.close();

    if(out.fail())
    {
        error = "Cannot write temp file: " + temp_path + "(" + std::strerror(errno) + ")";
        fs::remove(temp_path, ec);

        return false;
    }

    fs::rename(temp_path, path, ec);

    if(ec)
    {
        error = "Cannot replace file: " + full + "(" + ec.message() + ")";
        fs::remove(temp_path, ec);

        return false;
    }

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
