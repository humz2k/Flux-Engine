/*! \file log.h
 *  \brief Logging functions
 *
 *  Use these functions to print log messages. Each message has an
 *  associated log level:
 *
 *  CRITICAL: A critical unrecoverable error
 *  ERROR: A recoverable error
 *  WARNING: A warning
 *  INFO: High-level information about the progress of the application
 *  DEBUG: Lower-level information
 *  TRACE: Very low-level information.
 *
 */

#ifndef _HQ_LOG_H_
#define _HQ_LOG_H_

#include "raylib.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ERROR LOG_ERROR
#define WARNING LOG_WARNING
#define INFO LOG_INFO
#define DEBUG LOG_DEBUG
#define TRACE LOG_TRACE

/*void hq_set_log_level(hq_loglevel_t level);

void hq_log_detailed(hq_loglevel_t level, const char* func, const char* file,
                     int line, char* fmt, ...);*/

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define LOG(type, ...)                                                         \
    TraceLog(type,__VA_ARGS__)

#define PANIC_ON(condition, ...)                                               \
    if ((condition)) {                                                         \
        LOG(ERROR, __VA_ARGS__);                                               \
        exit(1);                                                               \
    }

//#define LOG_FUNC_CALL() LOG(DEBUG, "%s called", __func__)

#define LOG_FUNC_CALL()

#endif /* CHIRC_LOG_H_ */
