cmake_minimum_required(VERSION 2.8.3)
project(dr_log)

find_package(catkin REQUIRED COMPONENTS
	dr_cmake
	roscpp
	rosconsole
)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
)

dr_include_directories(
	include/${PROJECT_NAME}
	${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/dr_log.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.hpp"
)
