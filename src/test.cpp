#include "dr_log.hpp"

int main() {
	dr::setupLogging("./test/test.log", "test");
	DR_DEBUG("Debug");
	DR_INFO("Info");
	DR_SUCCESS("Succes");
	DR_WARN("Warning");
	DR_ERROR("Error");
	DR_CRITICAL("Critical");
}
