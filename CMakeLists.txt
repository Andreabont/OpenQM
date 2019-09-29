cmake_minimum_required(VERSION 2.6)
project(OpenQM)

# Project settings
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 

# Find boost::program_options
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# Uncomment to enable compiler warning
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Weffc++") 

# Enable test
enable_testing()
add_subdirectory(test)

# Source
set(LNS_SRCS 
${PROJECT_SOURCE_DIR}/implicant.h
${PROJECT_SOURCE_DIR}/implicant.cpp
${PROJECT_SOURCE_DIR}/qm.h
${PROJECT_SOURCE_DIR}/qm.cpp
${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(OpenQM ${LNS_SRCS})
TARGET_LINK_LIBRARIES(OpenQM ${Boost_LIBRARIES})
