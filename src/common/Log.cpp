#include "Log.h"

#include <map>
#include <chrono>

using namespace std;

const array<const string, 3> Log::sLogLevels = { "ERROR", "WARNING", "NOTICE" };

Log::Log(const string & fname) : mOutput(fname.c_str(), ios::out | ios::app) {
}

Log::Log(Log && other): mOutput(move(other.mOutput)) {
}

Log & Log::operator=(Log && other) {
    mOutput = move(other.mOutput);
    return *this;
}

std::ostream & Log::operator()(Log::Level lvl) {
    return mOutput << endl << sLogLevels[lvl] << " at [" << chrono::system_clock::now().time_since_epoch().count() << "] ";
}

Log & Log::Instance(const char * name) {
    static map<const char *, Log> logs;

    if (logs.find(name) == logs.end()) {
        logs.emplace(make_pair(name, Log(string(name) + ".log")));
    }
    return logs.at(name);
}
