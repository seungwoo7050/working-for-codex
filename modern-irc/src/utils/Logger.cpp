#include "Logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

Logger& Logger::getInstance() {
    static Logger instance;  // C++11+ guarantees thread-safe initialization
    return instance;
}

Logger::Logger() : minLevel(LOG_INFO), consoleOutput(true), fileOutput(false) {
}

Logger::~Logger() {
    closeLogFile();
}

std::string Logger::getCurrentTimestamp() const {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LOG_DEBUG:    return "DEBUG";
        case LOG_INFO:     return "INFO";
        case LOG_WARNING:  return "WARNING";
        case LOG_ERROR:    return "ERROR";
        case LOG_CRITICAL: return "CRITICAL";
        default:           return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    if (level < minLevel) {
        return;
    }

    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << std::setw(8) << std::left << levelToString(level) << "] "
        << message;

    std::string logLine = oss.str();

    if (consoleOutput) {
        if (level >= LOG_ERROR) {
            std::cerr << logLine << std::endl;
        } else {
            std::cout << logLine << std::endl;
        }
    }

    if (fileOutput && logFile.is_open()) {
        logFile << logLine << std::endl;
        logFile.flush();  // Ensure immediate write
    }
}

bool Logger::openLogFile(const std::string& filepath) {
    closeLogFile();
    logFilePath = filepath;
    logFile.open(filepath.c_str(), std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
        return false;
    }

    fileOutput = true;
    info("=== Log file opened: " + filepath + " ===");
    return true;
}

void Logger::closeLogFile() {
    if (logFile.is_open()) {
        info("=== Log file closed ===");
        logFile.close();
    }
    fileOutput = false;
}

void Logger::setMinLevel(LogLevel level) {
    minLevel = level;
}

void Logger::setConsoleOutput(bool enable) {
    consoleOutput = enable;
}

void Logger::setFileOutput(bool enable) {
    fileOutput = enable;
}

void Logger::debug(const std::string& message) {
    writeLog(LOG_DEBUG, message);
}

void Logger::info(const std::string& message) {
    writeLog(LOG_INFO, message);
}

void Logger::warning(const std::string& message) {
    writeLog(LOG_WARNING, message);
}

void Logger::error(const std::string& message) {
    writeLog(LOG_ERROR, message);
}

void Logger::critical(const std::string& message) {
    writeLog(LOG_CRITICAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    writeLog(level, message);
}

// Specialized logging methods
void Logger::logConnection(int fd, const std::string& hostname, int port) {
    std::ostringstream oss;
    oss << "New connection: fd=" << fd << " from " << hostname << ":" << port;
    info(oss.str());
}

void Logger::logDisconnection(int fd, const std::string& nickname, const std::string& reason) {
    std::ostringstream oss;
    oss << "Disconnection: fd=" << fd << " nick=" << nickname << " reason=" << reason;
    info(oss.str());
}

void Logger::logCommand(const std::string& nickname, const std::string& command, const std::string& params) {
    std::ostringstream oss;
    oss << "Command: " << nickname << " -> " << command;
    if (!params.empty()) {
        oss << " " << params;
    }
    debug(oss.str());
}

void Logger::logChannelJoin(const std::string& nickname, const std::string& channel) {
    std::ostringstream oss;
    oss << "JOIN: " << nickname << " joined " << channel;
    info(oss.str());
}

void Logger::logChannelPart(const std::string& nickname, const std::string& channel, const std::string& reason) {
    std::ostringstream oss;
    oss << "PART: " << nickname << " left " << channel;
    if (!reason.empty()) {
        oss << " (" << reason << ")";
    }
    info(oss.str());
}

void Logger::logKick(const std::string& kicker, const std::string& kicked, const std::string& channel, const std::string& reason) {
    std::ostringstream oss;
    oss << "KICK: " << kicker << " kicked " << kicked << " from " << channel;
    if (!reason.empty()) {
        oss << " (" << reason << ")";
    }
    info(oss.str());
}

void Logger::logModeChange(const std::string& setter, const std::string& target, const std::string& modes) {
    std::ostringstream oss;
    oss << "MODE: " << setter << " set " << modes << " on " << target;
    info(oss.str());
}
