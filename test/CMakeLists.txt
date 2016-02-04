cmake_minimum_required(VERSION 2.6)
project (test)

# Setup CMake to run tests
find_package(Boost COMPONENTS unit_test_framework system REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# Test source files
file(GLOB TEST_SRC RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

set(LNS_SRCS 
${PROJECT_SOURCE_DIR}/../implicant.h
${PROJECT_SOURCE_DIR}/../implicant.cpp
)

# Run through each source
foreach(testSrc ${TEST_SRC})

        # Log test
        message( STATUS "Loaded test case: " ${testSrc} )

        # Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        # Add compile target
        add_executable(${testName} ${testSrc} ${LNS_SRCS})

        # link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} ${Boost_LIBRARIES})

        # Add test
        add_test(NAME ${testName} 
                 WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                 COMMAND ${PROJECT_BINARY_DIR}/${testName})

endforeach(testSrc)
