#pragma once

#include <string>
#include <vector>

bool load_file(const char *path, std::vector<std::string> &lines);

bool save_file(const char *path, const std::vector<std::string> &lines, std::string& error);

//从路径中获取文件名
std::string get_filename(const char *path);