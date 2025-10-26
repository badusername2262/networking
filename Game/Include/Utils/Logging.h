#pragma once

#include <string>
#include <chrono>

// Define ANSI escape codes for text colors
#define ANSI_COLOUR_RED     "\x1b[31m"
#define ANSI_COLOUR_GREEN   "\x1b[32m"
#define ANSI_COLOUR_YELLOW  "\x1b[33m"
#define ANSI_COLOUR_RESET   "\x1b[0m"

// Define logging levels
#define LOG_LEVEL_INFO    0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR   2

// Define the current log level (change this as needed)
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

std::string format_timestamp()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time_t);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(timestamp);
}

// Define macros for logging at different levels
#define LOG_INFO_VAR(var)       if (LOG_LEVEL <= LOG_LEVEL_INFO)    std::cout << ANSI_COLOUR_GREEN << "[" << format_timestamp() << "] [INFO] " << var << ANSI_COLOUR_RESET << std::endl
#define LOG_WARN_VAR(var)       if (LOG_LEVEL <= LOG_LEVEL_WARNING) std::cout << ANSI_COLOUR_YELLOW << "[" << format_timestamp() << "] [WARNING] " << var << ANSI_COLOUR_RESET << std::endl
#define LOG_ERROR_VAR(var)      if (LOG_LEVEL <= LOG_LEVEL_ERROR)   std::cout << ANSI_COLOUR_RED << "[" << format_timestamp() << "] [ERROR] " << var << ANSI_COLOUR_RESET << std::endl
#define RUNTIME_ERROR_VAR(var)  if (LOG_LEVEL <= LOG_LEVEL_ERROR)   throw std::runtime_error( ANSI_COLOUR_RED "[" + format_timestamp() + "] [ERROR] " + __FILE__ + ": Line:" + std::to_string(__LINE__) + " - " + var + ANSI_COLOUR_RESET)

#define LOG_INFO(message)       if (LOG_LEVEL <= LOG_LEVEL_INFO)    std::cout << ANSI_COLOUR_GREEN << "[" << format_timestamp() << "] [INFO] " << message << ANSI_COLOUR_RESET << std::endl
#define LOG_WARN(message)       if (LOG_LEVEL <= LOG_LEVEL_WARNING) std::cout << ANSI_COLOUR_YELLOW << "[" << format_timestamp() << "] [WARNING] " << message << ANSI_COLOUR_RESET << std::endl
#define LOG_ERROR(message)      if (LOG_LEVEL <= LOG_LEVEL_ERROR)   std::cout << ANSI_COLOUR_RED << "[" << format_timestamp() << "] [ERROR] " << message << ANSI_COLOUR_RESET << std::endl
#define RUNTIME_ERROR(message)  if (LOG_LEVEL <= LOG_LEVEL_ERROR)   throw std::runtime_error( ANSI_COLOUR_RED "[" + format_timestamp() + "] [ERROR] " + __FILE__ + ": Line:" + std::to_string(__LINE__) + " - " + message + ANSI_COLOUR_RESET)

#define ASSERT(condition, message) while (false){ if (!(condition)) { throw std::runtime_error( ANSI_COLOUR_RED "[" + format_timestamp() + "] [ASSERTION FAILED] " + __FILE__ + ": Line:" + std::to_string(__LINE__) + " - " + message + ANSI_COLOUR_RESET)}