#include <ostream>
#include <ros/console.h>

#include <log4cxx/logger.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/dailyrollingfileappender.h>
#include <log4cxx/consoleappender.h>

namespace dr {

namespace {
	std::string const short_pattern = "[%5p] [%d{HH:mm:ss.SSS}] [%c] %m%n";
	std::string const full_pattern  = "[%5p] [%t] [%d{yyyy-MM-DD HH:mm:ss.SSS}] [%c] %F:%L: %m%n";
	std::string const console_pattern = "[%d{HH:mm:ss.SSS}] [%c] %m%n";

	/// Pattern layout that adds ASNI color codes based on log level.
	class AnsiColorPatternLayout : public log4cxx::PatternLayout {
	public:
		/// Construct a TerminalColorPatternLayout with a given pattern.
		AnsiColorPatternLayout(log4cxx::LogString const & pattern) : log4cxx::PatternLayout(pattern) {}

		// Produces a formatted string as specified by the conversion pattern.
		virtual void format(log4cxx::LogString &output, const log4cxx::spi::LoggingEventPtr &event, log4cxx::helpers::Pool &pool) const {
			// Make output red for error and above.
			if (event->getLevel()->isGreaterOrEqual(log4cxx::Level::getError())) {
				output.append("\x1b[31m");
			// Make output yellow for warnings and above, but below error.
			} else if (event->getLevel()->isGreaterOrEqual(log4cxx::Level::getWarn())) {
				output.append("\x1b[33m");
			}

			log4cxx::PatternLayout::format(output, event, pool);

			// Reset color.
			output.append("\x1b[39m");
		}
	};
}

void removeRosAppenders() {
	ROSCONSOLE_AUTOINIT;

	log4cxx::LoggerPtr root_logger = log4cxx::Logger::getLogger(ROSCONSOLE_ROOT_LOGGER_NAME);
	root_logger->removeAllAppenders();
}

void addDefaultAppenders(std::string const & base_dir) {
	ROSCONSOLE_AUTOINIT;

	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(ROSCONSOLE_DEFAULT_NAME);
	std::string logfile = base_dir + "/" + ROSCONSOLE_DEFAULT_NAME;

	log4cxx::AppenderPtr short_appender   = new log4cxx::DailyRollingFileAppender(new log4cxx::PatternLayout(short_pattern), logfile + ".short.log", "'.'yyyy-MM-dd");
	log4cxx::AppenderPtr long_appender    = new log4cxx::DailyRollingFileAppender(new log4cxx::PatternLayout(full_pattern),  logfile + ".full.log",  "'.'yyyy-MM-dd");
	log4cxx::AppenderPtr console_appender = new log4cxx::ConsoleAppender(new AnsiColorPatternLayout(console_pattern));

	logger->addAppender(short_appender);
	logger->addAppender(long_appender);
	logger->addAppender(console_appender);
}


void replaceDefaultAppenders(std::string const & base_dir) {
	removeRosAppenders();
	addDefaultAppenders(base_dir);
}

}
