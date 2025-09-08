cmake_minimum_required(VERSION 3.24)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

set(SYLAR_TEST ON)

if (BUILD_SHARED_LIBS)
    set(SYLAR_LIB_TYPE SHARED)
    message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
else()
    set(SYLAR_LIB_TYPE STATIC)
    message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
endif()

project(Sylar
    DESCRIPTION "sylar framework"
    LANGUAGES CXX C
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "Setting build type to `Debug` as none was specified.")
endif()

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)

if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
        message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake ..\" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif()

add_definitions(-DTOP_DIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)
include(CTest)

include(cmake/version/version.cmake)
include(cmake/utils.cmake)
include(cmake/thirdparty/yaml.cmake)

# ZLIB
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

# PTHREAD
find_library(PTHREAD pthread)

# BOOST
set(Boost_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/boost-install)
find_package(Boost 1.80.0 REQUIRED)

# RAGEL
find_program(RAGEL_BIN ragel PATHS ${PROJECT_SOURCE_DIR}/thirdparty/ragel-install/bin)
if(NOT RAGEL_BIN)
    message(FATAL_ERROR "Ragel not found")
endif()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

message(STATUS "Build mode: ${CMAKE_BUILD_TYPE}")

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -fnon-call-exceptions -fomit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# Output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes
set(SYLAR_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/source/include/)
set(SYLAR_THIRDPARTY_INCLUDE_DIR ${Boost_INCLUDE_DIRS})
get_target_property(YAML_INCLUDE_DIRS yaml-cpp INCLUDE_DIRECTORIES)

include_directories(
    ${SYLAR_INCLUDE_DIR}
    ${SYLAR_THIRDPARTY_INCLUDE_DIR}
    ${YAML_INCLUDE_DIRS}
)

# #####################################################################################################################
# Other CMake modules
# MUST BE ADDED AFTER CONFIGURING COMPILER PARAMETERS
# #####################################################################################################################

add_subdirectory(source)

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################、

if(SYLAR_TEST)
    sylar_add_executable(test_log "tests/test.cc" sylar sylar)
    sylar_add_executable(test_config "tests/test_config.cc" sylar sylar)
    sylar_add_executable(test_thread "tests/test_thread.cc" sylar sylar)
    sylar_add_executable(test_fiber "tests/test_fiber.cc" sylar sylar)
    sylar_add_executable(test_scheduler "tests/test_scheduler.cc" sylar sylar)
    sylar_add_executable(test_iomanager "tests/test_iomanager.cc" sylar sylar)
    sylar_add_executable(test_hook "tests/test_hook.cc" sylar sylar)
    sylar_add_executable(test_address "tests/test_address.cc" sylar sylar)
    sylar_add_executable(test_socket "tests/test_socket.cc" sylar sylar)
    sylar_add_executable(test_bytearray "tests/test_bytearray.cc" sylar sylar)
    sylar_add_executable(test_http "tests/test_http.cc" sylar sylar)
    sylar_add_executable(test_http_parser "tests/test_http_parser.cc" sylar sylar)
    sylar_add_executable(test_tcp_server "tests/test_tcp_server.cc" sylar sylar)
    sylar_add_executable(test_http_server "tests/test_http_server.cc" sylar sylar)
    sylar_add_executable(test_http_connection "tests/test_http_connection.cc" sylar sylar)
    sylar_add_executable(test_uri "tests/test_uri.cc" sylar sylar)
    sylar_add_executable(test_daemon "tests/test_daemon.cc" sylar sylar)
    sylar_add_executable(test_env "tests/test_env.cc" sylar sylar)
    sylar_add_executable(test_application "tests/test_application.cc" sylar sylar)
endif()

    sylar_add_executable(echo_server "examples/echo_server.cc" sylar sylar)

# #####################################################################################################################
# INSTALL TARGETS
# #####################################################################################################################

# 默认安装路径
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install_sylar" CACHE PATH "Install path prefix" FORCE)
endif()

# 安装导出配置
install(EXPORT SylarTargets
    FILE SylarTargets.cmake
    NAMESPACE Sylar::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Sylar
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install/SylarConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SylarConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Sylar
)

# 生成版本文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SylarConfigVersion.cmake
    VERSION ${SYLAR_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装配置文件
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/SylarConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SylarConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Sylar
)