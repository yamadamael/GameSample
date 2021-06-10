#include <stdarg.h>
#include <string>
#include <sstream>
#include <fstream>
#include "Logger.h"

std::string LogLevel::ToString(LogLevel::type logLevel)
{
    switch (logLevel)
    {
    case LogLevel::type::Info:
        return "INFO ";
    case LogLevel::type::Debug:
        return "DEBUG";
    case LogLevel::type::Warn:
        return "WARN ";
    case LogLevel::type::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

Logger *Logger::GetInstance()
{
    static Logger instance;
    return &instance;
}

// 出力したいログレベルを設定します
void Logger::SetLogLevel(const LogLevel::type &logLevel)
{
    this->m_tatgetLogLevel = logLevel;
}

// 出力したいログファイルを設定します
void Logger::SetLogFilePath(const char *logFilePath)
{
    this->m_logFilePath = logFilePath;
}

void Logger::Write(const LogLevel::type &logLevel, const char *fileName, const char *funcName, const int lineNum, const char *format, ...)
{
    char message[512] = {0};
    va_list args;

    va_start(args, format);
#ifdef _WIN32
    vsprintf_s(message, format, args);
#else
    vsprintf(message, format, args);
#endif
    this->write(logLevel, fileName, funcName, lineNum, message);
    va_end(args);
}

void Logger::Info(const char *const fileName, const char *const funcName, const int lineNum, const char *message) // const
{
    this->write(LogLevel::type::Info, fileName, funcName, lineNum, message);
}

void Logger::Debug(const char *const fileName, const char *const funcName, const int lineNum, const char *message) // const
{
    this->write(LogLevel::type::Debug, fileName, funcName, lineNum, message);
}

void Logger::Warn(const char *const fileName, const char *const funcName, const int lineNum, const char *message) // const
{
    this->write(LogLevel::type::Warn, fileName, funcName, lineNum, message);
}

void Logger::Error(const char *const fileName, const char *const funcName, const int lineNum, const char *message) // const
{
    this->write(LogLevel::type::Error, fileName, funcName, lineNum, message);
}

//
Logger::Logger() : m_tatgetLogLevel(LogLevel::type::Info)
{
    // ログファイル名を決めましょう
    this->m_logFilePath = "Log/testlog.log";
}

void Logger::write(const LogLevel::type &logLevel, const char *fileName, const char *funcName, const int lineNum, const char *message) // const
{
    if (!this->isEnabled(logLevel))
        return;

    // ログを出力する処理
    std::ofstream ofs;
    ofs.open(this->m_logFilePath, std::ios::app);

    ofs << this->getDateTimeNow() << " "
        << "[" << LogLevel::ToString(logLevel) << "]"
        << "[" << fileName << "]"
        << "[" << funcName << "]"
        << "[" << lineNum << "]"
        << message
        << std::endl;

    ofs.close();
}

bool Logger::isEnabled(const LogLevel::type &logLevel) // const
{
    return this->m_tatgetLogLevel <= logLevel;
}

std::string Logger::getDateTimeNow() // const
{
    std::ostringstream oss;

    // 時刻を整形する処理

    return oss.str();
}
