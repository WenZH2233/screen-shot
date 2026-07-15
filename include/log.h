#pragma once
#include <string>
#include <thread>
#include <fstream>
#include <mutex>

namespace core{
enum class Level {
    INFO,
    WARNING,
    ERROR
};
enum class op{
    endl,
    time
};
class _log{
    std::string cache;
    std::thread logThread;
    std::ofstream logFile;
    std::mutex logMutex;
public:
    _log();
    ~_log();
	void exit();
    void init();
    void log(const std::string& message);
    void log(op operation);
    _log& level(Level level);
    _log& operator<<(Level level);
    _log& operator<<(op operation);
    _log& operator<<(const std::string& message);
    _log& operator<<(const char message);
    _log& operator<<(const unsigned char message);
    _log& operator<<(const short message);
    _log& operator<<(const unsigned short message);
    _log& operator<<(const int message);
    _log& operator<<(const unsigned int message);
    _log& operator<<(const long message);
    _log& operator<<(const unsigned long message);
    _log& operator<<(const long long message);
    _log& operator<<(const unsigned long long message);
    _log& operator<<(const float message);
    _log& operator<<(const double message);
};
extern _log Log;
}