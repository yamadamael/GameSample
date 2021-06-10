#include <stdarg.h>
#include <string>
#include <sstream>
#include <fstream>

class LogLevel
{
public:
    enum class type
    {
        Info = 1,
        Debug,
        Warn,
        Error,
    };

    static std::string ToString(type logLevel);
};

class Logger
{
private:
    LogLevel::type m_tatgetLogLevel;
    std::string m_logFilePath;

public:
    static Logger *GetInstance();

    void SetLogLevel(const LogLevel::type &logLevel);

    void SetLogFilePath(const char *logFilePath);

    void Write(const LogLevel::type &logLevel, const char *fileName, const char *funcName, const int lineNum, const char *format, ...);

private:
    void Info(const char *const fileName, const char *const funcName, const int lineNum, const char *message);

    void Debug(const char *const fileName, const char *const funcName, const int lineNum, const char *message);

    void Warn(const char *const fileName, const char *const funcName, const int lineNum, const char *message);

    void Error(const char *const fileName, const char *const funcName, const int lineNum, const char *message);

    Logger();

    void write(const LogLevel::type &logLevel, const char *fileName, const char *funcName, const int lineNum, const char *message);

    bool isEnabled(const LogLevel::type &logLevel);

    std::string getDateTimeNow();
};

#define LOG_LEVEL_SET(x) Logger::GetInstance()->SetLogLevel(x);
#define LOG_FILE_PATH_SET(x) Logger::GetInstance()->SetLogFilePath(x);

#ifdef _WIN32
#define LOG_INFO(format, ...) Logger::GetInstance()->Write(LogLevel::type::Info, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOG_DEBUG(format, ...) Logger::GetInstance()->Write(LogLevel::type::Debug, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOG_WARN(format, ...) Logger::GetInstance()->Write(LogLevel::type::Warn, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#define LOG_ERROR(format, ...) Logger::GetInstance()->Write(LogLevel::type::Error, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__);
#else
#define LOG_INFO(format, ...) Logger::GetInstance()->Write(LogLevel::type::Info, __FILE__, __func__, __LINE__, format, __VA_ARGS__);
#define LOG_DEBUG(format, ...) Logger::GetInstance()->Write(LogLevel::type::Debug, __FILE__, __func__, __LINE__, format, __VA_ARGS__);
#define LOG_WARN(format, ...) Logger::GetInstance()->Write(LogLevel::type::Warn, __FILE__, __func__, __LINE__, format, __VA_ARGS__);
#define LOG_ERROR(format, ...) Logger::GetInstance()->Write(LogLevel::type::Error, __FILE__, __func__, __LINE__, format, __VA_ARGS__);
#endif