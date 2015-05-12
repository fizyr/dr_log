#include <utility>
#include <type_traits>
#include <iomanip>

#include "dr_log.hpp"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/filesystem.hpp>


namespace dr {

namespace {
	namespace log      = boost::log;
	namespace keywords = boost::log::keywords;

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
			LogLevel severity = record["Severity"].extract_or_default<LogLevel, void, LogLevel>(LogLevel::info);

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
				case LogLevel::critical:
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
		mapping[LogLevel::critical] = log::sinks::syslog::critical;
		return mapping;
	};
}

std::ostream & operator<< (std::ostream & stream, LogLevel level) {
	static char const * strings[] = {
		"debug",
		"info",
		"success",
		"warning",
		"error",
		"critical",
	};

	std::size_t numeric = int(level);
	if (numeric < sizeof(strings) / sizeof(strings[0])) {
		stream << strings[numeric];
	} else {
		stream << "unknown";
	}

	return stream;
}


void setupLogging(std::string const & base_dir, std::string const & name) {
	log::core_ptr core = log::core::get();

	// Get base and node directories.
	boost::filesystem::path base = base_dir;
	boost::filesystem::path node = base / name;

	// Create directories if needed.
	boost::system::error_code error;
	boost::filesystem::create_directories(base, error);
	boost::filesystem::create_directories(node, error);

	// Add common attributes.
	log::add_common_attributes();
	core->add_global_attribute("Node", log::attributes::constant<std::string>(name));

	// Text format for file and console log.
	auto text_format = log::expressions::stream
		<< "[" << log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "] "
		<< "[" << std::setw(8) << std::left << log::expressions::attr<LogLevel>("Severity") << "] "
		<< "[" << log::expressions::attr<std::string>("Node") << "] "
		<< log::expressions::message;

	// Add file sink.
	log::add_file_log(
		keywords::file_name = (node / (name + ".log")).native(),
		keywords::open_mode = std::ios_base::out | std::ios_base::app,
		keywords::format    = text_format
	);

	// Add console sink.
	log::add_console_log(std::clog)->set_formatter(makeAnsiColorFormatter(text_format));

	// Add syslog sink.
	auto syslog = boost::make_shared<log::sinks::syslog_backend>(
		keywords::facility = log::sinks::syslog::user,
		keywords::use_impl = log::sinks::syslog::native
	);

	syslog->set_severity_mapper(severityMapping());
	core->add_sink(boost::make_shared<log::sinks::synchronous_sink<log::sinks::syslog_backend>>(syslog));
}

}
