#include "dr_log.hpp"

int main() {
	dr::setupLogging("./test/test.log", "test");
	DR_DEBUG("Debug" << " message");
	DR_INFO("Info" << " message");
	DR_SUCCESS("Succes" << " message");
	DR_WARN("Warning" << " message");
	DR_ERROR("Error" << " message");
	DR_CRITICAL("Critical" << " message");
}
