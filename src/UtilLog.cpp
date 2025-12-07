#include "UtilLog.h"
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

std::ofstream UtilLog::logFile;
std::mutex UtilLog::logMutex;
bool UtilLog::isInitialized = false;

bool UtilLog::InitLogSystem(const std::string &filename)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (!isInitialized)
    {
        logFile.open(filename, std::ios::out | std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            return false;
        }

        isInitialized = true;

        logFile << "=====================================" << std::endl;
        logFile << "Log Started at " << GetTimestamp() << std::endl;
        logFile << "=====================================" << std::endl;
        logFile.flush();

        InfoF("Log system initialized. Writing to file: %s", filename.c_str());
    }
    return true;
}

void UtilLog::CloseLogSystem()
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (isInitialized && logFile.is_open())
    {
        logFile << "=====================================" << std::endl;
        logFile << "Log Ended at " << GetTimestamp() << std::endl;
        logFile << "=====================================" << std::endl;
        logFile.close();
        isInitialized = false;
    }
}

void UtilLog::Info(const char *message)
{
    SimOneAPI::SetLogOut(ESimOne_LogLevel_Type::ESimOne_LogLevel_Type_Information, message);
}

void UtilLog::Warning(const char *message)
{
    SimOneAPI::SetLogOut(ESimOne_LogLevel_Type::ESimOne_LogLevel_Type_Warning, message);
}

void UtilLog::Error(const char *message)
{
    SimOneAPI::SetLogOut(ESimOne_LogLevel_Type::ESimOne_LogLevel_Type_Error, message);
}

void UtilLog::Debug(const char *message)
{
    SimOneAPI::SetLogOut(ESimOne_LogLevel_Type::ESimOne_LogLevel_Type_Debug, message);
}

void UtilLog::InfoF(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Info(buffer);
}

void UtilLog::WarningF(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Warning(buffer);
}

void UtilLog::ErrorF(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Error(buffer);
}

void UtilLog::DebugF(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Debug(buffer);
}
void UtilLog::WriteToFile(const char *level, const char *message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (isInitialized && logFile.is_open())
    {
        logFile << "[" << GetTimestamp() << "] [" << level << "] " << message << std::endl;
        logFile.flush();
    }
}

std::string UtilLog::GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}