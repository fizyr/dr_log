#include "log4cxx.hpp"
#include "dr_log.hpp"

#include <log4cxx/appenderskeleton.h>
#include <log4cxx/logger.h>

namespace dr {

namespace {
	LogLevel fromLog4Cxx(log4cxx::Level const & level) {
		switch (level.toInt()) {
			case log4cxx::Level::DEBUG_INT: return LogLevel::debug;
			case log4cxx::Level::TRACE_INT: return LogLevel::debug;
			case log4cxx::Level::INFO_INT:  return LogLevel::info;
			case log4cxx::Level::WARN_INT:  return LogLevel::warning;
			case log4cxx::Level::ERROR_INT: return LogLevel::error;
			case log4cxx::Level::FATAL_INT: return LogLevel::fatal;
			default: return LogLevel::info;
		}
	}

	/// Appender for log4cxx to pipe log messages to the DR log.
	class DrLog4cxxAppender : public log4cxx::AppenderSkeleton {
	protected:
		/// If true, also log a warning whenever a log4cxx message is captured.
		bool warn;

	public:
		/// Construct a log4cxx to DR  appender.
		DrLog4cxxAppender(bool warn) : warn(warn) {}

		/// Append a log event to the log.
		virtual void append(log4cxx::spi::LoggingEventPtr const & event, log4cxx::helpers::Pool &) override {
			log4cxx::spi::LocationInfo const & location = event->getLocationInformation();
			(void) location;

			DR_LOG_AT(fromLog4Cxx(*event->getLevel()), location->getFileName(), location->getLineNumber(), event->getMessage());
			if (warn) {
				DR_LOG_AT(LogLevel::warning, location->getFileName(), location->getLineNumber(), "Message received via log4cxx. Did you forget to switch to DR logging macros?");
			}
		}

		/// Close the appender.
		virtual void close() override {}

		/// Does the appender need a layout?
		virtual bool requiresLayout() const override { return false; }
	};
}

/// Register an appender with log4cxx to pipe all messages to the DR log.
void registerLog4cxxAppenders() {
	log4cxx::LoggerPtr root_logger = log4cxx::Logger::getRootLogger();
	root_logger->addAppender(new DrLog4cxxAppender(true));
}

}
