#include "dr_log.hpp"

int main() {
	dr::setupLogging("./test/test.log", "test");
	boost::log::sources::severity_logger<dr::LogLevel> logger;
	BOOST_LOG_SEV(logger, dr::LogLevel::debug)    << "Debug";
	BOOST_LOG_SEV(logger, dr::LogLevel::info)     << "Info";
	BOOST_LOG_SEV(logger, dr::LogLevel::success)  << "Succes";
	BOOST_LOG_SEV(logger, dr::LogLevel::warning)  << "Warning";
	BOOST_LOG_SEV(logger, dr::LogLevel::error)    << "Error";
	BOOST_LOG_SEV(logger, dr::LogLevel::critical) << "Critical";
}
