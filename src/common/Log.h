#ifndef _LOG_H_INCLUDED_
#define _LOG_H_INCLUDED_

#include <fstream>
#include <array>
#include <string>

class Log {
public:
    enum Level { ERROR, WARNING, NOTICE };
    static const std::array<const std::string, 3> sLogLevels;
private:
    std::ofstream mOutput;


    Log(const std::string & fname);
public:

    Log(const Log &) = delete;
    Log & operator=(const Log &) = delete;

    Log(Log &&);
    Log & operator=(Log &&);

    std::ostream & operator()(Log::Level lvl);

    static Log & Instance(const char * name);
};

#endif // _LOG_H_INCLUDED_