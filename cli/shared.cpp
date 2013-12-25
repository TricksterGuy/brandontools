#include "shared.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

std::string ToUpper(const std::string& str)
{
    std::string cap = str;
    transform(cap.begin(), cap.end(), cap.begin(), (int(*)(int)) std::toupper);
    return cap;
}

void split(const std::string& s, char delimiter, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delimiter))
        tokens.push_back(item);
}

void Chop(std::string& filename)
{
    int index = filename.rfind('\\');
    if ((unsigned int)index == std::string::npos)
    {
        index = filename.rfind('/');
        if ((unsigned int)index == std::string::npos) index = -1;
    }

    filename = filename.substr(index+1);
}

std::string Sanitize(const std::string& filename)
{
    std::stringstream out;
    for (unsigned int i = 0; i < filename.size(); i++)
    {
        if ((filename[i] >= 'A' && filename[i] <= 'Z') ||
            (filename[i] >= 'a' && filename[i] <= 'z') ||
            (filename[i] >= '0' && filename[i] <= '9') ||
            filename[i] == '_')
            out.put(filename[i]);
    }
    return out.str();
}

std::string Format(const std::string& file)
{
    std::string filename = file;
    Chop(filename);
    return Sanitize(filename);
}
