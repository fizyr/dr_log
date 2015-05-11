#include <utility>

#include "dr_log.hpp"
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/filesystem.hpp>


namespace dr {

namespace log      = boost::log;
namespace keywords = boost::log::keywords;

template<typename CharT>
struct AnsiColorFormatter {
	log::formatter slave;
	using return_type = void;

	AnsiColorFormatter(log::formatter const & slave) : slave(slave) {}
	AnsiColorFormatter(log::formatter && slave) : slave(std::move(slave)) {}

	void operator() (log::record_view const & record, log::basic_formatting_ostream<CharT> & stream) {
		LogLevel severity = record["Severity"].extract_or_default<LogLevel, void, LogLevel>(LogLevel::info);

		// Error and up is red.
		if (severity >= LogLevel::error) {
			stream << "\x1b[31m";

		// Warning is yellow.
		} else if (severity >= LogLevel::warning) {
			stream << "\x1b[33m";

		// Success is green.
		} else if (severity == LogLevel::success) {
			stream << "\x1b[32m";

		// Rest is default color.
		} else {
			stream << "\x1b[39m";
		}

		slave(record, stream);

		// Reset color.
		stream << "\x1b[39m";
	}

};


void setupLogging(std::string const & base_dir, std::string const & name) {
	log::core_ptr core = log::core::get();

	boost::filesystem::path base = base_dir;
	boost::filesystem::path node = base / name;

	boost::system::error_code error;
	boost::filesystem::create_directories(base, error);
	boost::filesystem::create_directories(node, error);

	log::add_common_attributes();

	auto text_format = log::expressions::format("[%1%] %2%")
		% log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
		% log::expressions::message;

	log::add_file_log(
		keywords::file_name = (node / (name + ".log")).native(),
		keywords::open_mode = std::ios_base::out | std::ios_base::app,
		keywords::format    = text_format
	);

	log::add_console_log(std::clog)->set_formatter(AnsiColorFormatter<char>(text_format));

	auto syslog = boost::make_shared<log::sinks::syslog_backend>(
		keywords::facility = log::sinks::syslog::user,
		keywords::use_impl = log::sinks::syslog::native
	);

	core->add_sink(boost::make_shared<log::sinks::synchronous_sink<log::sinks::syslog_backend>>(syslog));
}

}
