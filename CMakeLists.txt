cmake_minimum_required(VERSION 2.8.12)
project(CFACTER)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Defaulting to a release build.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Find Boost
find_package(Boost 1.48 COMPONENTS program_options system filesystem)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost 1.48 or newer is required.  Please install it before building.")
endif()

# Find Log4cxx
find_package(LOG4CXX)
if (NOT LOG4CXX_FOUND)
    message(FATAL_ERROR "Log4cxx version 10.0 is required.  Please install it before building.")
endif()

# Find RE2
find_package(RE2)
if (NOT RE2_FOUND)
    message(FATAL_ERROR "RE2 is required.  Please install it before building.")
endif()

# Find openssl
find_package(OPENSSL)
if (NOT OPENSSL_FOUND)
    message(FATAL_ERROR "openssl is required.  Please install it before building.")
endif()

# Find YAMLCPP
find_package(YAMLCPP)
if (NOT YAMLCPP_FOUND)
    message(FATAL_ERROR "yaml-cpp is required.  Please install it before building.")
endif()

# Set RPATH if not installing to a system library directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" INSTALL_IS_SYSTEM_DIR)
if ("${INSTALL_IS_SYSTEM_DIR}" STREQUAL "-1")
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# Include vendor libraries
add_subdirectory(vendor/gmock-1.7.0)
set(RAPIDJSON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/vendor/rapidjson-0.11/include")

add_subdirectory(exe)

#
# Add cpplint target
#
include(FindPythonInterp)
if (NOT PYTHONINTERP_FOUND)
    message(STATUS "Python not found; 'cpplint' target will not be available")
else()
    set(CPPLINT_FILTER
        "-build/include"          # Why?
        "-build/namespaces"       # What's a namespace to do
        "-legal/copyright"        # Not yet
        "-runtime/references"     # Not sure about this religion
        "-readability/streams"    # What?
        "-readability/namespace"  # Ignore nested namespace comment formatting
        "-whitespace/braces"      # Is there a k&r setting?
        "-whitespace/line_length" # Well yeah, but ... not just now
        "-runtime/arrays"         # Sizing an array with a 'const int' doesn't make it variable sized
        "-readability/todo"       # Seriously? todo comments need to identify an owner? pffft
        "-whitespace/empty_loop_body" # Can't handle do { ... } while(expr);
    )

    file(GLOB_RECURSE ALL_SOURCES lib/*.cc lib/*.h lib/*.hpp exe/*.cc exe/*.h exe/*.hpp)

    set(CPPLINT_PATH "${PROJECT_SOURCE_DIR}/scripts/cpplint.py")

    set(CPPLINT_ARGS "--extensions=cc,hpp,h;--root=lib/inc")
    if (CPPLINT_FILTER)
        string(REPLACE ";" "," CPPLINT_FILTER "${CPPLINT_FILTER}")
        set(CPPLINT_ARGS "${CPPLINT_ARGS};--filter=${CPPLINT_FILTER}")
    endif()
    if (MSVC)
        set(CPPLINT_ARGS "${CPPLINT_ARGS};--output=vs7")
    endif()

    add_custom_target(cpplint
        COMMAND ${PYTHON_EXECUTABLE} ${CPPLINT_PATH} ${CPPLINT_ARGS} ${ALL_SOURCES}
        VERBATIM
    )
endif()

add_custom_target(cppcheck
    COMMAND cppcheck lib exe
)

# Add test executables for unit testing
add_test("library\\ tests" "${PROJECT_BINARY_DIR}/lib/tests/libfacter_test" "--gtest_color=yes")
