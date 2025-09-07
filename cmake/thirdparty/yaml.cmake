message(STATUS "----------- ADD YAML -----------")

set(YAML_SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp-src)
set(YAML_BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/yaml-cpp-build)
set(YAML_SUBBUILD_DIR ${CMAKE_BINARY_DIR}/thirdparty/yaml-cpp-subbuild)

if(EXISTS ${YAML_SOURCE_DIR}/CMakeLists.txt)
    message(STATUS "Using existing yaml in ${YAML_SOURCE_DIR}")
    add_subdirectory(${YAML_SOURCE_DIR} ${YAML_BINARY_DIR})
else()
    message(STATUS "Local yaml not found, downloading...")

    include(FetchContent)
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY git@github.com:jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.7.0
        SOURCE_DIR ${YAML_SOURCE_DIR}
        BINARY_DIR ${YAML_BINARY_DIR}
        SUBBUILD_DIR ${YAML_SUBBUILD_DIR}
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD TRUE
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()

message(STATUS "----------- FINISH ADD YAML -----------")