set(SYLAR_VERSION_MAJOR 0)
set(SYLAR_VERSION_MINOR 0)
set(SYLAR_VERSION_PATCH 0)
set(SYLAR_VERSION_TWEAK 1)

set(SYLAR_VERSION
    "${SYLAR_VERSION_MAJOR}.${SYLAR_VERSION_MINOR}.${SYLAR_VERSION_PATCH}.${SYLAR_VERSION_TWEAK}"
)

set(PROJECT_VERSION ${SYLAR_VERSION})
set(PROJECT_VERSION_MAJOR ${SYLAR_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${SYLAR_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${SYLAR_VERSION_PATCH})
set(PROJECT_VERSION_TWEAK ${SYLAR_VERSION_TWEAK})

find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H
    OUTPUT_VARIABLE SYLAR_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
if("${SYLAR_GIT_HASH}" STREQUAL "")
  set(SYLAR_GIT_HASH "unknown")
endif()

string(SUBSTRING ${SYLAR_GIT_HASH} 0 8 SYLAR_SHORT_GIT)


string(TIMESTAMP SYLAR_BUILD_TIME "%Y-%m-%d %H:%M:%S")
string(TIMESTAMP SYLAR_TIME_SUFFIX "%Y%m%d_%H%M%S")

set(SYLAR_SUFFIX ${SYLAR_VERSION}.${SYLAR_SHORT_GIT}.${SYLAR_TIME_SUFFIX})

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(SYLAR_SUFFIX "${SYLAR_SUFFIX}-ea")
endif()

message(STATUS "------------ Sylar Project ---------")
message(STATUS "\t  Version : ${SYLAR_VERSION}")
message(STATUS "\t      Git : ${SYLAR_GIT_HASH}")
message(STATUS "\tBuildTime : ${SYLAR_BUILD_TIME}")
message(STATUS "------------------------------------")

configure_file("${PROJECT_SOURCE_DIR}/cmake/version/version.hpp.in"
               "${PROJECT_SOURCE_DIR}/source/include/version.hpp" @ONLY)

configure_file("${PROJECT_SOURCE_DIR}/cmake/version/version.cpp.in"
               "${PROJECT_SOURCE_DIR}/source/src/version.cpp" @ONLY)