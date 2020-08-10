# dr_log

This packages implements a wrapper for `boost::logging`.
Its interface resembles that of ROS (ie. it uses `DR_INFO("Hello World");` versus `ROS_INFO("Hello World");`).

## Sinks

Sinks in `boost::logging` terms are ways to output the data.
By default, `dr_log` uses 2 sinks:

- `syslog`
- `console` (makes use of `std::clog`, which is comparable to `std::cerr`)

The `syslog` sink can be disabled by setting the `DR_LOG_USE_SYSLOG` environment variable to `0`.
The `console` sink can be disabled by setting the `DR_LOG_USE_CONSOLE` environment variable to `0`.

## Systemd

`dr_log` has a Systemd logging mode. This allows for console logging without colors and timestamps,
and with log-level prefixed in a way that systemd parses when `SyslogLevelPrefix=` is enabled.

This mode can be enabled by setting the `DR_LOG_CONSOLE_FORMAT` environment variable to `systemd`.
This only works with the 'console' sink enabled.

## Example

```c++
#include <thread>
#include <dr_log/dr_log.hpp>

int main() {
	dr::setupLogging("./test/test.log", "test");
	DR_DEBUG("Debug" << " message");
	DR_INFO("Info" << " message");
	DR_SUCCESS("Succes" << " message");
	DR_WARN("Warning" << " message");
	DR_ERROR("Error" << " message");
	DR_FATAL("Critical" << " message");

	for (int i = 0; i < 10; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		DR_INFO_THROTTLE(2, "Throttled" << " at 2 Hz: " << i);
	}
}
```
