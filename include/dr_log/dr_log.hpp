#include <string>

namespace dr {

/// Remove the default ROS appenders.
void removeRosAppenders();

/// Add the default Delft Robotics appenders for log4cxx.
void addDefaultAppenders(std::string const & base_dir);

/// Remove the default ROS appenders and set the appenders to the default Delft Robotics appenders.
void replaceDefaultAppenders(std::string const & base_dir);

}
