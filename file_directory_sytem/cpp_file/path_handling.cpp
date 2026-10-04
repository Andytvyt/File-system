#include "..\h_file\system.h"

using namespace std;


bool System::isValidPath(const string& path)
{
    if (path.empty())
        return false;

    if (path.front() == '/' || path.back() == '/')
        return false;

    if (path.find("//") != string::npos)
        return false;

    string invalidChars = "<>:\"|?*";

    for (char c : path)
    {
        if (invalidChars.find(c) != string::npos)
            return false;
    }

    return true;
}


string System::getFileName(const string& path)
{
    if (!isValidPath(path))
        return "";

    size_t pos = path.find_last_of('/');

    if (pos == string::npos)
        return path;

    return path.substr(pos + 1);
}


string System::getExtension(const string& path)
{
    string fileName = getFileName(path);

    if (fileName.empty())
        return "";

    size_t pos = fileName.find_last_of('.');

    if (pos == string::npos)
        return "";

    if (pos == 0 || pos == fileName.length() - 1)
        return "";

    return fileName.substr(pos + 1);
}


string System::getParentPath(const string& path)
{
    if (!isValidPath(path))
        return "";

    size_t pos = path.find_last_of('/');

    if (pos == string::npos)
        return "";

    return path.substr(0, pos);
}

