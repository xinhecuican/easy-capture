#ifndef ENUMREFLECT_H
#define ENUMREFLECT_H
#include <string>
#include <sstream>
#include<QString>
#include<QDebug>
// Search and remove whitespace from both ends of the string
static std::string TrimEnumString(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it)) { it++; }
    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit)) { rit++; }
    return std::string(it, rit.base());
}

static void SplitEnumArgs(const char* szArgs, std::string Array[], int nMax)
{
    std::stringstream ss(szArgs);
    std::string strSub;
    int nIdx = 0;
    while (ss.good() && (nIdx < nMax)) {
        getline(ss, strSub, ',');
        if(strSub.find("end") != strSub.npos ||
                (strSub.substr(strSub.size()-5, 5) == "begin" && strSub.find("=") == strSub.npos))
        {
            continue;
        }
        if(strSub.substr(strSub.size()-5, 5) == "begin")
        {
            int end_place = 0;
            for(int i=1; i<(int)strSub.length(); i++)
            {
                if(strSub[i] == ' ' || strSub[i] == '=')
                {
                    end_place = i;
                    break;
                }
            }
            Array[nIdx] = TrimEnumString(strSub.substr(0, end_place));
            nIdx++;
        }
        else
        {
            Array[nIdx] = TrimEnumString(strSub);
            nIdx++;
        }
    }
};
// This will to define an enum that is wrapped in a namespace of the same name along with ToString(), FromString(), and COUNT
#define MAKE_ENUM(ename, ...) \
    enum ename { __VA_ARGS__, COUNT }; \
    static std::string _Strings[COUNT]; \
    static const QString eto_string(ename e) { \
    if (_Strings[0].empty()) { SplitEnumArgs(#__VA_ARGS__, _Strings, COUNT); } \
    return QString::fromStdString(_Strings[e]); \
    } \
    static ename efrom_string(const std::string& strEnum) { \
    if (_Strings[0].empty()) { SplitEnumArgs(#__VA_ARGS__, _Strings, COUNT); } \
    for (int i = 0; i < COUNT; i++) { if (_Strings[i] == strEnum) { return (ename)i; } } \
    return COUNT; \
    } \

#define DEFINE_STRING(class_name)\
    std::string class_name::_Strings[class_name::COUNT];
/*
 * usage:
 * MAKE_ENUM(name, type1, type2, ...)
 *
 * 之后如果是在类中定义的，还需要在类外声明一下_Strings，可以使用DEFINE_STRING宏
 */
#endif // ENUM_HELPER_H
