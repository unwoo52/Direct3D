#include "EngineUtils.h"

string EngineUtils::GetDirectoryFromPath(const string& filepath)
{
    size_t off1 = filepath.find_last_of('\\');
    size_t off2 = filepath.find_last_of('/');
    if (off1 == string::npos && off2 == string::npos)
    {
        return {};
    }
    if (off1 == string::npos)
    {
        return filepath.substr(0, off2);
    }
    if (off2 == string::npos)
    {
        return filepath.substr(0, off1);
    }
    return filepath.substr(0, off1 > off2 ? off1 : off2);
}
