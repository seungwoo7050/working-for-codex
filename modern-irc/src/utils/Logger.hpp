#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <ctime>

// Log levels
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
};

class Logger {
private:
    std::ofstream logFile;
    LogLevel minLevel;
    bool consoleOutput;
    bool fileOutput;
    std::string logFilePath;

    Logger();
    std::string getCurrentTimestamp() const;
    std::string levelToString(LogLevel level) const;
    void writeLog(LogLevel level, const std::string& message);

public:
    ~Logger();

    // Non-copyable, non-movable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // Singleton access (thread-safe in C++11+)
    static Logger& getInstance();

    // Configuration
    bool openLogFile(const std::string& filepath);
    void closeLogFile();
    void setMinLevel(LogLevel level);
    void setConsoleOutput(bool enable);
    void setFileOutput(bool enable);

    // Logging methods
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    void log(LogLevel level, const std::string& message);

    // Specialized logging
    void logConnection(int fd, const std::string& hostname, int port);
    void logDisconnection(int fd, const std::string& nickname, const std::string& reason);
    void logCommand(const std::string& nickname, const std::string& command, const std::string& params);
    void logChannelJoin(const std::string& nickname, const std::string& channel);
    void logChannelPart(const std::string& nickname, const std::string& channel, const std::string& reason);
    void logKick(const std::string& kicker, const std::string& kicked, const std::string& channel, const std::string& reason);
    void logModeChange(const std::string& setter, const std::string& target, const std::string& modes);
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_CRITICAL(msg) Logger::getInstance().critical(msg)

#endif
