cmake_minimum_required(VERSION 2.8.3)
project(dr_log)

find_package(catkin REQUIRED COMPONENTS
	dr_cmake
)

find_package(Boost REQUIRED COMPONENTS log log_setup system date_time)
find_package(PThread REQUIRED)

catkin_package(
	INCLUDE_DIRS include ${Boost_INCLUDE_DIRS}
	LIBRARIES ${PROJECT_NAME} ${Boost_LIBRARIES}
)

dr_include_directories(
	include/${PROJECT_NAME}
	${catkin_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/dr_log.cpp
)

add_executable(${PROJECT_NAME}_test src/test.cpp)

target_compile_options(${PROJECT_NAME}      PUBLIC ${PThread_COMPILE_OPTIONS})
target_compile_options(${PROJECT_NAME}_test PUBLIC ${PThread_COMPILE_OPTIONS})

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${Boost_LIBRARIES} ${PThread_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} ${PThread_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.hpp"
)
