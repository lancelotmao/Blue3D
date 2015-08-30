#include <algorithm>
#include "HouyiStringUtils.h"

namespace Houyi
{
    bool StringUtils::replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return replace(str, from, to);
    }
    
    string StringUtils::getLastPathComponent(const string& path)
    {
        int pos = path.find_last_of("\\");
        if (pos == string::npos)
        {
            pos = path.find_last_of("/");
        }
        if (pos != string::npos)
        {
            return path.substr(pos + 1);
        }
        return path;
    }

    string StringUtils::deleteLastPathComponent(const string& path)
    {
        int length = path.size();
        if (length > 0)
        {
            string p = path;
            if (p[length - 1] == '/')
            {
                p = p.substr(0, length - 1);
            }

            int pos = p.find_last_of("/");
            if (pos != string::npos)
            {
                return p.substr(0, pos + 1);
            }
        }
        return path;
    }

    string StringUtils::getPath(const string& full)
    {
        int pos = full.find_last_of("\\");
        if (pos == string::npos)
        {
            pos = full.find_last_of("/");
        }
        if (pos != string::npos)
        {
            return full.substr(0, pos + 1);
        }
        return full;
    }

    string StringUtils::getTexturePath(const string& filePath, const string& texturePath)
    {
        string fp = getPath(filePath);
        string tp = texturePath;
        replace(tp, "%20", " ");

        // first remove file:/// as we are always in local
        std::string protocalPrefix("file:///");
        int protocalPrefixLength = protocalPrefix.size();
        if (!tp.compare(0, protocalPrefixLength, protocalPrefix))
        {
            tp = tp.substr(protocalPrefixLength);
        }

        // handle parent folder
        string prefix("../");
        while (!tp.compare(0, prefix.size(), prefix))
        {
            fp = deleteLastPathComponent(fp);
            tp = tp.substr(3);
        }

        string prefix2("./");
        while (!tp.compare(0, prefix2.size(), prefix2))
        {
            fp = deleteLastPathComponent(fp);
            tp = tp.substr(2);
        }
        return fp + tp;
    }

    bool StringUtils::endsWith(const string& fullString, const string& ending)
    {
        return (fullString.length() >= ending.length())
                && (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }
    
    string StringUtils::append(string src, float number)
    {
        char buffer [32];
        sprintf (buffer, "%f", number);
        return src + buffer;
    }
    
    string StringUtils::append(string src, int number)
    {
        char buffer [32];
        sprintf (buffer, "%d", number);
        return src + buffer;
    }

    string StringUtils::removeSuffix(const string& src)
    {
        int pos = src.find_last_of(".");
        if (pos != string::npos)
        {
            return src.substr(0, pos);
        }
        return src;
    }
    
    string StringUtils::getSuffix(const string& src)
    {
        int pos = src.find_last_of(".");
        if (pos != string::npos)
        {
            return src.substr(pos + 1);
        }
        return src;
    }

    char easytolower(char in)
    {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    }

    bool StringUtils::hasSuffix(const string& src ,const string& suffix)
    {
        string data = src;
        std::transform(data.begin(), data.end(), data.begin(), easytolower);
        string s = suffix;
        std::transform(s.begin(), s.end(), s.begin(), easytolower);
        return endsWith(data, s);
    }
}
