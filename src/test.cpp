#include <thread>
#include "dr_log.hpp"

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
