#include "log.h"

#include "base.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace core{
    _log Log;
    _log::_log() {}
    _log::~_log() {}
    void _log::exit(){
        if (logThread.joinable()) {
            logThread.join();
        }
        if (!cache.empty()) {
            logFile << cache;
            cache.clear();
        }
        logFile.close();
    }
    void _log::init() {
        std::string logPath = dataPath+"log.log";
        std::filesystem::path logDir = std::filesystem::path(logPath).parent_path();
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }
        logFile.open(logPath, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logPath << std::endl;
            return;
        }
        logThread = std::thread([this]() {
            while (!g_Quited) {
                if (!cache.empty()) {
                    logFile << cache;
                    cache.clear();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
        logThread.detach();
        Log<<Level::INFO<<"Log initialized at "<<logPath<<op::endl;
    }
    void _log::log(const std::string& message) {
        cache += message;
#ifdef _DEBUG
        std::cout << message;
#endif
    }
    void _log::log(op operation) {
        switch (operation) {
            case op::endl:
                log("\n");
                logMutex.unlock();
                break;
            case op::time: {
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                char timeStr[100];
                if (std::strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", std::localtime(&now_c))) {
                    this->log(timeStr);
                }
                break;
            }
        }
    }
    _log& _log::level(Level level) {
        logMutex.lock();
        this->log(op::time);
        switch (level) {
            case Level::INFO:
                this->log("[INFO] ");
                break;
            case Level::WARNING:
                this->log("[WARNING] ");
                break;
            case Level::ERROR:
                this->log("[ERROR] ");
                break;
        }
        return *this;
    }
    _log& _log::operator<<(Level level) {
        return this->level(level);
    }
    _log& _log::operator<<(op operation) {
        this->log(operation);
        return *this;
    }
    _log& _log::operator<<(const std::string& message) {
        this->log(message);
        return *this;
    }
    _log& _log::operator<<(const char message) {
        this->log(std::to_string(static_cast<int>(message)));
        return *this;
    }
    _log& _log::operator<<(const unsigned char message) {
        this->log(std::to_string(static_cast<int>(message)));
        return *this;
    }
    _log& _log::operator<<(const short message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const unsigned short message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const int message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const unsigned int message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const long message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const unsigned long message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const long long message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const unsigned long long message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const float message) {
        this->log(std::to_string(message));
        return *this;
    }
    _log& _log::operator<<(const double message) {
        this->log(std::to_string(message));
        return *this;
    }
	_log& _log::operator<<(const std::vector<int>& message) {
		std::string vecStr = "[";
		for (size_t i = 0; i < message.size(); ++i) {
			vecStr += std::to_string(message[i]);
			if (i < message.size() - 1) {
				vecStr += ", ";
			}
		}
		vecStr += "]";
		this->log(vecStr);
		return *this;
	}
}