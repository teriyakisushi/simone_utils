#pragma once

#include "SimOneServiceAPI.h"
#include <string>
#include <fstream>
#include <mutex>
class UtilLog
{
public:
    /**
     * 初始化日志系统
     * @param filename 日志文件名
     * @return 是否初始化成功
     */
    static bool InitLogSystem(const std::string &filename = "sleepfox_log.txt");

    /**
     * 关闭日志系统
     */
    static void CloseLogSystem();

    /**
     * 输出信息级别的日志
     * @param message 日志消息
     */
    static void Info(const char *message);

    /**
     * 输出警告级别的日志
     * @param message 日志消息
     */
    static void Warning(const char *message);

    /**
     * 输出错误级别的日志
     * @param message 日志消息
     */
    static void Error(const char *message);

    /**
     * 输出调试级别的日志
     * @param message 日志消息
     */
    static void Debug(const char *message);

    /**
     * 格式化输出信息级别的日志
     * @param format 格式化字符串
     * @param ... 可变参数列表
     */
    static void InfoF(const char *format, ...);

    /**
     * 格式化输出警告级别的日志
     * @param format 格式化字符串
     * @param ... 可变参数列表
     */
    static void WarningF(const char *format, ...);

    /**
     * 格式化输出错误级别的日志
     * @param format 格式化字符串
     * @param ... 可变参数列表
     */
    static void ErrorF(const char *format, ...);

    /**
     * 格式化输出调试级别的日志
     * @param format 格式化字符串
     * @param ... 可变参数列表
     */
    static void DebugF(const char *format, ...);

private:
    /**
     * 输出日志
     * @param level 日志级别
     * @param message 日志消息
     */
    static void WriteToFile(const char *level, const char *message);

    /**
     * 获取当前时间戳
     * @return 格式化的时间字符串
     */
    static std::string GetTimestamp();

    static std::ofstream logFile;
    static std::mutex logMutex;
    static bool isInitialized;
};
