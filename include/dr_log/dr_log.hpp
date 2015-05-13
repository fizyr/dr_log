#pragma once
#define BOOST_LOG_USE_NATIVE_SYSLOG
#define BOOST_LOG_DYN_LINK

#include <string>
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
	critical,
};

/// Output a log level to a stream.
std::ostream & operator<< (std::ostream & stream, LogLevel level);

using Logger = boost::log::sources::severity_logger_mt<LogLevel>;

BOOST_LOG_GLOBAL_LOGGER(dr_logger, Logger);

#define DR_DEBUG(msg)    BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::debug)    << msg;
#define DR_INFO(msg)     BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::info)     << msg;
#define DR_SUCCESS(msg)  BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::success)  << msg;
#define DR_WARN(msg)     BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::warning)  << msg;
#define DR_ERROR(msg)    BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::error)    << msg;
#define DR_CRITICAL(msg) BOOST_LOG_SEV(::dr::dr_logger::get(), ::dr::LogLevel::critical) << msg;
#define DR_FATAL(msg)    DR_CRITICAL(msg)

#define DR_STRINGIFY(x) #x
#define DR_ASSERT(cond)  do { if (cond) break; DR_FATAL("ASSERTION FAILED\n\tfile = " DR_STRINGIFY(__FILE__) " \n\tline = " DR_STRINGIFY(__LINE__) "\n\tcond = " #cond"\n"); assert(false); } while (0)


// Initialize the logging library.
void setupLogging(std::string const & log_file, std::string const & name);

}
