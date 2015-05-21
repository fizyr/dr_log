#include "dr_log.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/attributes/value_extraction.hpp>

#include <utility>
#include <type_traits>
#include <iomanip>

namespace dr {

BOOST_LOG_GLOBAL_LOGGER_DEFAULT(dr_logger, Logger);

namespace {
	namespace log      = boost::log;
	namespace keywords = boost::log::keywords;

	/// Convert a boost shared pointer to a weak pointer.
	template<typename T>
	boost::weak_ptr<T> sharedToWeak(boost::shared_ptr<T> const & shared) {
		return boost::weak_ptr<T>(shared);
	}

	/// Convert a boost weak pointer to a shared pointer.
	template<typename T>
	boost::shared_ptr<T> weakToShared(boost::weak_ptr<T> const & shared) {
		return boost::shared_ptr<T>(shared);
	}

	struct NullDeleter {
		void operator() (void const *) const {}
	};

	/// Tag for formatting severyity level in log output.
	struct LevelTag;

	/// Format a severity level for log output.
	boost::log::formatting_ostream & operator<< (boost::log::formatting_ostream & stream,  log::to_log_manip<LogLevel, LevelTag> const & level) {
		static char const * strings[] = {
			"DEBUG",
			"INFO",
			"GOOD",
			"WARN",
			"ERROR",
			"FATAL",
		};

		std::size_t numeric = int(level.get());
		if (numeric < sizeof(strings) / sizeof(strings[0])) {
			stream << strings[numeric];
		} else {
			stream << "?????";
		}

		return stream;
	}

	/// Formatter that invokes a slave formatter and adds ANSI color codes based on log severity.
	template<typename Slave>
	class AnsiColorFormatter {
	protected:
		Slave slave;

	public:
		/// Construct an AnsiColorFormatter with a slave formatter.
		AnsiColorFormatter(Slave const & slave) : slave(slave) {}

		/// Construct an AnsiColorFormatter with a slave formatter.
		AnsiColorFormatter(Slave && slave) : slave(std::move(slave)) {}

		/// Format the record.
		void operator() (log::record_view const & record, log::basic_formatting_ostream<char> & stream) {
			LogLevel severity = log::extract_or_default<LogLevel>("Severity", record, LogLevel::info);

			// Add color code.
			switch (severity) {
				case LogLevel::debug:
					stream << "\x1b[1;30m";
					break;
				case LogLevel::info:
					stream << "\x1b[0m";
					break;
				case LogLevel::success:
					stream << "\x1b[32m";
					break;
				case LogLevel::warning:
					stream << "\x1b[33m";
					break;
				case LogLevel::error:
					stream << "\x1b[31m";
					break;
				case LogLevel::fatal:
					stream << "\x1b[1;31m";
					break;
				default:
					break;
			}

			// Invoke slave.
			slave(record, stream);

			// Reset color.
			stream << "\x1b[0m";
		}

	};

	/// Create an AnsiColorFormatter with a slave formatter.
	template<typename Slave>
	AnsiColorFormatter<typename std::decay<Slave>::type> makeAnsiColorFormatter(Slave && slave) {
		return AnsiColorFormatter<typename std::decay<Slave>::type>(std::forward<Slave>(slave));
	}

	/// Get a syslog severity mapper for dr::LogLevel.
	log::sinks::syslog::custom_severity_mapping<LogLevel> severityMapping(std::string const & attribute_name = "Severity") {
		log::sinks::syslog::custom_severity_mapping<LogLevel> mapping(attribute_name);
		mapping[LogLevel::debug]    = log::sinks::syslog::debug;
		mapping[LogLevel::info]     = log::sinks::syslog::info;
		mapping[LogLevel::success]  = log::sinks::syslog::info;
		mapping[LogLevel::warning]  = log::sinks::syslog::warning;
		mapping[LogLevel::error]    = log::sinks::syslog::error;
		mapping[LogLevel::fatal]    = log::sinks::syslog::critical;
		return mapping;
	};

	// Text format for file and console log.
	auto text_format = log::expressions::stream
		<< "[" << log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "] "
		<< "[" << std::setw(5) << std::right << log::expressions::attr<LogLevel, LevelTag>("Severity") << "] "
		<< "[" << log::expressions::attr<std::string>("Node") << "] "
		<< log::expressions::message;

	// Create a console sink.
	boost::shared_ptr<log::sinks::synchronous_sink<log::sinks::basic_text_ostream_backend<char>>> createConsoleSink() {
		auto backend = boost::make_shared<log::sinks::text_ostream_backend>();
		backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, NullDeleter()));
		auto frontend = boost::make_shared<log::sinks::synchronous_sink<log::sinks::text_ostream_backend>>(backend);
		frontend->set_formatter(makeAnsiColorFormatter(text_format));
		return frontend;
	}

	// Create a syslog sink.
	boost::shared_ptr<log::sinks::synchronous_sink<log::sinks::syslog_backend>> createSyslogSink() {
		// Add syslog sink.
		auto syslog = boost::make_shared<log::sinks::syslog_backend>(
			keywords::facility = log::sinks::syslog::user,
			keywords::use_impl = log::sinks::syslog::native
		);

		syslog->set_severity_mapper(severityMapping());
		return boost::make_shared<log::sinks::synchronous_sink<log::sinks::syslog_backend>>(syslog);
	}


	// Create a file sink that removes itself on exceptions and then logs the exception.
	boost::shared_ptr<log::sinks::synchronous_sink<log::sinks::text_file_backend>> createFileSink(std::string const & filename) {
		// Add file sink.
		try {
			auto file_frontend = boost::make_shared<log::sinks::synchronous_sink<log::sinks::text_file_backend>>(
				keywords::file_name = filename,
				keywords::open_mode = std::ios_base::out | std::ios_base::app
			);

			file_frontend->set_formatter(text_format);

			auto weak = sharedToWeak(file_frontend);
			auto exception_handler = [filename, weak] (std::exception const & e) {
				log::core_ptr core = log::core::get();
				log::core::get()->remove_sink(weakToShared(weak));
				DR_ERROR("Error writing log file: `" << filename << "' " << e.what());
			};

			file_frontend->set_exception_handler(log::make_exception_handler<std::exception>(exception_handler));
			return file_frontend;

		} catch (std::exception const & e) {
			DR_ERROR("Failed to create log file `" << filename << "': " << e.what());
			return nullptr;
		}
	}
}

std::ostream & operator<< (std::ostream & stream, LogLevel level) {
	static char const * strings[] = {
		"debug",
		"info",
		"success",
		"warning",
		"error",
		"fatal",
	};

	std::size_t numeric = int(level);
	if (numeric < sizeof(strings) / sizeof(strings[0])) {
		stream << strings[numeric];
	} else {
		stream << "unknown";
	}

	return stream;
}

void setupLogging(std::string const & log_file, std::string const & name) {
	log::core_ptr core = log::core::get();

	// Add common attributes.
	log::add_common_attributes();
	core->add_global_attribute("Node", log::attributes::constant<std::string>(name));

	// Add  sinks.
	core->add_sink(createConsoleSink());
	core->add_sink(createSyslogSink());
	core->add_sink(createFileSink(log_file));
}

}
