/*
 * Log.h
 *
 *  Created on: 2013-2-1
 *      Author: fasiondog
 */

#pragma once
#ifndef HIKUU_LOG_H_
#define HIKUU_LOG_H_

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "config.h"
#include "exception.h"

// clang-format off
#if USE_SPDLOG_LOGGER
    #include <spdlog/spdlog.h>
    #include <spdlog/fmt/ostr.h>
    #if HKU_USE_SPDLOG_ASYNC_LOGGER
        #include "spdlog/async.h"
    #endif
#endif
// clang-format on

#include <fmt/ostream.h>
#include <fmt/format.h>
#include <fmt/chrono.h>

#ifndef HKU_API
#define HKU_API
#endif

namespace hku {

/**
 * @ingroup Utilities
 * @addtogroup logging Logging tools 日志工具
 * @details 打印等级：
 * TRACE < DEBUG < INFO < WARN < ERROR < FATAL
 * @{
 */

/**********************************************
 * Use SPDLOG for logging
 *********************************************/
#if USE_SPDLOG_LOGGER
/** 日志级别 */
enum LOG_LEVEL {
    TRACE = SPDLOG_LEVEL_TRACE,     ///< 跟踪
    DEBUG = SPDLOG_LEVEL_DEBUG,     ///< 调试
    INFO = SPDLOG_LEVEL_INFO,       ///< 一般信息
    WARN = SPDLOG_LEVEL_WARN,       ///< 告警
    ERROR = SPDLOG_LEVEL_ERROR,     ///< 错误
    FATAL = SPDLOG_LEVEL_CRITICAL,  ///< 致命
    OFF = SPDLOG_LEVEL_OFF,         ///< 关闭日志打印
};

/**
 * 获取当前日志级别
 * @return
 */
LOG_LEVEL HKU_API get_log_level();

/**
 * 设置日志级别
 * @param level 指定的日志级别
 */
void HKU_API set_log_level(LOG_LEVEL level);

std::shared_ptr<spdlog::logger> HKU_API getHikyuuLogger();

#define HKU_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#define HKU_TRACE(...) SPDLOG_LOGGER_TRACE(hku::getHikyuuLogger(), __VA_ARGS__)

#define HKU_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#define HKU_DEBUG(...) SPDLOG_LOGGER_DEBUG(hku::getHikyuuLogger(), __VA_ARGS__)

#define HKU_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#define HKU_INFO(...) SPDLOG_LOGGER_INFO(hku::getHikyuuLogger(), __VA_ARGS__)

#define HKU_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#define HKU_WARN(...) SPDLOG_LOGGER_WARN(hku::getHikyuuLogger(), __VA_ARGS__)

#define HKU_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#define HKU_ERROR(...) SPDLOG_LOGGER_ERROR(hku::getHikyuuLogger(), __VA_ARGS__)

#define HKU_LOGGER_FATAL(logger, ...) \
    SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)
#define HKU_FATAL(...) SPDLOG_LOGGER_CRITICAL(hku::getHikyuuLogger(), __VA_ARGS__)

#if HKU_USE_SPDLOG_ASYNC_LOGGER
void initLogger();
#else
void initLogger();
#endif

#else
enum LOG_LEVEL {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
    OFF = 6,
};

LOG_LEVEL HKU_API get_log_level();
void HKU_API set_log_level(LOG_LEVEL level);
void initLogger();

/** 获取系统当前时间，精确到毫秒，如：2001-01-02 13:01:02.001 */
std::string HKU_API getLocalTime();

#if LOG_ACTIVE_LEVEL <= 0
#define HKU_TRACE(...)                                                                            \
    fmt::print("[{}] [HKU-T] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_TRACE(...)
#endif

#if LOG_ACTIVE_LEVEL <= 1
#define HKU_DEBUG(...)                                                                            \
    fmt::print("[{}] [HKU-D] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_DEBUG(...)
#endif

#if LOG_ACTIVE_LEVEL <= 2
#define HKU_INFO(...)                                                                             \
    fmt::print("[{}] [HKU-I] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_INFO(...)
#endif

#if LOG_ACTIVE_LEVEL <= 3
#define HKU_WARN(...)                                                                             \
    fmt::print("[{}] [HKU-W] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_WARN(...)
#endif

#if LOG_ACTIVE_LEVEL <= 4
#define HKU_ERROR(...)                                                                            \
    fmt::print("[{}] [HKU-E] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_ERROR(...)
#endif

#if LOG_ACTIVE_LEVEL <= 5
#define HKU_FATAL(...)                                                                            \
    fmt::print("[{}] [HKU-F] - {} ({}:{})\n", getLocalTime(), fmt::format(__VA_ARGS__), __FILE__, \
               __LINE__);
#else
#define HKU_FATAL(...)
#endif

#endif /* USE_SPDLOG_LOGGER */

///////////////////////////////////////////////////////////////////////////////
//
// clang/gcc 下使用 __PRETTY_FUNCTION__ 会包含函数参数，可以在编译时指定
// #define HKU_FUNCTION __PRETTY_FUNCTION__
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HKU_FUNCTION
#define HKU_FUNCTION __FUNCTION__
#endif

/**
 * 若表达式为 false，将抛出 hku::exception 异常, 并附带传入信息
 * @note 用于外部入参及结果检查
 */
#define HKU_CHECK(expr, ...)                                                                   \
    do {                                                                                       \
        if (!(expr)) {                                                                         \
            throw hku::exception(fmt::format("CHECK({}) {} [{}] ({}:{})", #expr,               \
                                             fmt::format(__VA_ARGS__), __FUNCTION__, __FILE__, \
                                             __LINE__));                                       \
        }                                                                                      \
    } while (0)

/**
 * 若表达式为 false，将抛出指定的异常, 并附带传入信息
 * @note 用于外部入参及结果检查
 */
#define HKU_CHECK_THROW(expr, except, ...)                                                      \
    do {                                                                                        \
        if (!(expr)) {                                                                          \
            throw except(fmt::format("{} [{}] ({}:{})", fmt::format(__VA_ARGS__), __FUNCTION__, \
                                     __FILE__, __LINE__));                                      \
        }                                                                                       \
    } while (0)

#if HKU_DISABLE_ASSERT
#define HKU_ASSERT(expr)
#define HKU_ASSERT_M(expr, ...)

#else /* #if HKU_DISABLE_ASSERT */

/**
 * 若表达式为 false，将抛出 hku::exception 异常
 * @note 仅用于内部入参检查，编译时可通过 HKU_DISABLE_ASSERT 宏关闭
 */
#define HKU_ASSERT(expr)                                                                        \
    do {                                                                                        \
        if (!(expr)) {                                                                          \
            throw hku::exception(                                                               \
              fmt::format("ASSERT({}) [{}] ({}:{})", #expr, __FUNCTION__, __FILE__, __LINE__)); \
        }                                                                                       \
    } while (0)

/**
 * 若表达式为 false，将抛出 hku::exception 异常, 并附带传入信息
 * @note 仅用于内部入参检查，编译时可通过 HKU_DISABLE_ASSERT 宏关闭
 */
#define HKU_ASSERT_M(expr, ...)                                                                \
    do {                                                                                       \
        if (!(expr)) {                                                                         \
            throw hku::exception(fmt::format("ASSERT({}) {} [{}] ({}:{})", #expr,              \
                                             fmt::format(__VA_ARGS__), __FUNCTION__, __FILE__, \
                                             __LINE__));                                       \
        }                                                                                      \
    } while (0)

#endif /* #if HKU_DISABLE_ASSERT */

/** 抛出 hku::exception 及传入信息 */
#define HKU_THROW(...)                                                                           \
    do {                                                                                         \
        throw hku::exception(fmt::format("EXCEPTION: {} [{}] ({}:{})", fmt::format(__VA_ARGS__), \
                                         __FUNCTION__, __FILE__, __LINE__));                     \
    } while (0)

/** 抛出指定异常及传入信息 */
#define HKU_THROW_EXCEPTION(except, ...)                                                    \
    do {                                                                                    \
        throw except(fmt::format("{} [{}] ({}:{})", fmt::format(__VA_ARGS__), __FUNCTION__, \
                                 __FILE__, __LINE__));                                      \
    } while (0)

/**
 * 满足指定条件时，打印 TRACE 信息
 * @param expr 指定条件
 */
#define HKU_TRACE_IF(expr, ...) \
    if (expr) {                 \
        HKU_TRACE(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，打印 DEBUG 信息, 并返回指定值
 * @param expr 指定条件
 */
#define HKU_DEBUG_IF(expr, ...) \
    if (expr) {                 \
        HKU_DEBUG(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，打印 INFO 信息, 并返回指定值
 * @param expr 指定条件
 */
#define HKU_INFO_IF(expr, ...) \
    if (expr) {                \
        HKU_INFO(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，打印 WARN 信息, 并返回指定值
 * @param expr 指定条件
 */
#define HKU_WARN_IF(expr, ...) \
    if (expr) {                \
        HKU_WARN(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，打印 ERROR 信息, 并返回指定值
 * @param expr 指定条件
 */
#define HKU_ERROR_IF(expr, ...) \
    if (expr) {                 \
        HKU_ERROR(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，打印 FATAL 信息, 并返回指定值
 * @param expr 指定条件
 */
#define HKU_FATAL_IF(expr, ...) \
    if (expr) {                 \
        HKU_FATAL(__VA_ARGS__); \
    }

/**
 * 满足指定条件时，返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_IF_RETURN(expr, ret, ...) \
    if (expr) {                       \
        return ret;                   \
    }

/**
 * 满足指定条件时，打印 TRACE 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_TRACE_IF_RETURN(expr, ret, ...) \
    if (expr) {                             \
        HKU_TRACE(__VA_ARGS__);             \
        return ret;                         \
    }

/**
 * 满足指定条件时，打印 DEBUG 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_DEBUG_IF_RETURN(expr, ret, ...) \
    if (expr) {                             \
        HKU_DEBUG(__VA_ARGS__);             \
        return ret;                         \
    }

/**
 * 满足指定条件时，打印 INFO 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_INFO_IF_RETURN(expr, ret, ...) \
    if (expr) {                            \
        HKU_INFO(__VA_ARGS__);             \
        return ret;                        \
    }

/**
 * 满足指定条件时，打印 WARN 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_WARN_IF_RETURN(expr, ret, ...) \
    if (expr) {                            \
        HKU_WARN(__VA_ARGS__);             \
        return ret;                        \
    }

/**
 * 满足指定条件时，打印 ERROR 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_ERROR_IF_RETURN(expr, ret, ...) \
    if (expr) {                             \
        HKU_ERROR(__VA_ARGS__);             \
        return ret;                         \
    }

/**
 * 满足指定条件时，打印 FATAL 信息, 并返回指定值
 * @param expr 指定条件
 * @param ret 返回值
 */
#define HKU_FATAL_IF_RETURN(expr, ret, ...) \
    if (expr) {                             \
        HKU_FATAL(__VA_ARGS__);             \
        return ret;                         \
    }

/** @} */

} /* namespace hku */

#endif /* HIKUU_LOG_H_ */
