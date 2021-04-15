cmake_minimum_required(VERSION 3.15)

project(jsoncpp VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(JSONCPP_USE_EXCEPTIONS "Build JSONCpp with exception." ON)
option(BUILD_STATIC_LIB "Build JSONCpp as a static lib." ON)
option(BUILD_SHARED_LIB "Build JSONCpp as a shared lib." ON)
option(BUILD_EXAMPLES "Build JSONCpp examples." ON)
option(BUILD_TESTS "Build JSONCpp unit tests." OFF)

# JSONCpp use exception
if (JSONCPP_USE_EXCEPTIONS)
    add_definitions(-DJSON_USE_EXCEPTION)
endif()

# Build examples
if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif(BUILD_EXAMPLES)

# Build unit test
if(BUILD_TESTS)
    # enable_testing()
    # include(CTest)
endif(BUILD_TESTS)

# Install include files
add_subdirectory(include)

# Build diff lib
add_subdirectory(src)
