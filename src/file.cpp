#include "file.hpp"
#include <fstream>

std::string File::content(const std::string &path)
{
    std::ostringstream buffer;
    std::ifstream input(path);

    buffer << input.rdbuf();
    return buffer.str();
}
