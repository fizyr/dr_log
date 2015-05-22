#pragma once
#include <chrono>
#include <string>

#define BOOST_LOG_USE_NATIVE_SYSLOG
#define BOOST_LOG_DYN_LINK

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace dr {

/// Available log levels.
enum class LogLevel {
	debug = 0,
	info,
	success,
	warning,
	error,
	fatal,
};

/// Output a log level to a stream.
std::ostream & operator<< (std::ostream & stream, LogLevel level);

using Logger = boost::log::sources::severity_logger_mt<LogLevel>;

BOOST_LOG_GLOBAL_LOGGER(dr_logger, Logger);

#define DR_LOG_AT(severity, file, line, msg) BOOST_LOG_SEV(::dr::dr_logger::get(), severity) << msg;

#define DR_LOG(severity, msg) DR_LOG_AT(severity, __FILE__, __LINE__, msg);

#define DR_LOG_THROTTLE(severity, rate, msg) do { \
	static ::std::chrono::steady_clock::time_point _dr_log__last; \
	auto _dr_log__now   = ::std::chrono::steady_clock::now(); \
	auto _dr_log__delay = ::std::chrono::microseconds(1000000/ rate); \
	if (_dr_log__now - _dr_log__last < _dr_log__delay) break; \
	_dr_log__last = _dr_log__now; \
	DR_LOG(severity, msg); \
} while(false)

#define DR_DEBUG(msg)    DR_LOG(::dr::LogLevel::debug,     msg);
#define DR_INFO(msg)     DR_LOG(::dr::LogLevel::info,      msg);
#define DR_SUCCESS(msg)  DR_LOG(::dr::LogLevel::success,   msg);
#define DR_WARN(msg)     DR_LOG(::dr::LogLevel::warning,   msg);
#define DR_ERROR(msg)    DR_LOG(::dr::LogLevel::error,     msg);
#define DR_FATAL(msg)    DR_LOG(::dr::LogLevel::fatal,     msg);

#define DR_DEBUG_THROTTLE(rate, msg)    DR_LOG_THROTTLE(::dr::LogLevel::debug,    rate, msg);
#define DR_INFO_THROTTLE(rate, msg)     DR_LOG_THROTTLE(::dr::LogLevel::info,     rate, msg);
#define DR_SUCCESS_THROTTLE(rate, msg)  DR_LOG_THROTTLE(::dr::LogLevel::success,  rate, msg);
#define DR_WARN_THROTTLE(rate, msg)     DR_LOG_THROTTLE(::dr::LogLevel::warning,  rate, msg);
#define DR_ERROR_THROTTLE(rate, msg)    DR_LOG_THROTTLE(::dr::LogLevel::error,    rate, msg);
#define DR_FATAL_THROTTLE(rate, msg)    DR_LOG_THROTTLE(::dr::LogLevel::fatal,    rate, msg);

#define DR_STRINGIFY(x) #x
#define DR_ASSERT(cond)  do { if (cond) break; DR_FATAL("ASSERTION FAILED\n\tfile = " DR_STRINGIFY(__FILE__) " \n\tline = " DR_STRINGIFY(__LINE__) "\n\tcond = " #cond"\n"); assert(false); } while (0)


// Initialize the logging library.
void setupLogging(std::string const & log_file, std::string const & name);

void registerLog4cxxAppenders();

}
