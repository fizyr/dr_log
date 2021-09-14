cmake_minimum_required(VERSION 3.8)
project(dr_log)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)
add_link_options(LINKER:--as-needed)

find_package(catkin REQUIRED COMPONENTS)

find_package(Boost REQUIRED COMPONENTS log log_setup system date_time thread)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)

catkin_package(
	INCLUDE_DIRS include ${Boost_INCLUDE_DIRS}
	LIBRARIES ${PROJECT_NAME} ${Boost_LIBRARIES}
)

include_directories(include/${PROJECT_NAME})
include_directories(SYSTEM
	${catkin_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} src/dr_log.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads ${log4cxx_LIBRARIES})

add_executable(${PROJECT_NAME}_test src/test.cpp)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} Threads::Threads)

install(
	TARGETS "${PROJECT_NAME}"
	ARCHIVE DESTINATION "${CATKIN_PACKAGE_LIB_DESTINATION}"
	LIBRARY DESTINATION "${CATKIN_PACKAGE_LIB_DESTINATION}"
	RUNTIME DESTINATION "${CATKIN_PACKAGE_BIN_DESTINATION}"
	COMPONENT runtime
)

install(
	DIRECTORY "include/${PROJECT_NAME}/"
	DESTINATION "${CATKIN_PACKAGE_INCLUDE_DESTINATION}"
	COMPONENT development
)
