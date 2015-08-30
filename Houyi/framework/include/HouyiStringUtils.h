#ifndef __Houyi__HouyiStringUtils__
#define __Houyi__HouyiStringUtils__

#include <string>

using namespace std;

namespace Houyi
{
    class StringUtils
    {
    public:
        static bool replace(std::string& str, const std::string& from, const std::string& to);
        static string getLastPathComponent(const string& path);
        static string deleteLastPathComponent(const string& path);
        static string getPath(const string& full);
        static string getTexturePath(const string& filePath, const string& texturePath);
        static bool endsWith(const string& fullString, const string& ending);
        static string append(string src, float number);
        static string append(string src, int number);
        static string removeSuffix(const string& src);
        static string getSuffix(const string& src);
        static bool hasSuffix(const string& src ,const string& suffix);
    };
}

#endif
