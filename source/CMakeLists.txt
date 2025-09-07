add_subdirectory(src/app)
add_subdirectory(src/common)
add_subdirectory(src/config)
add_subdirectory(src/daemon)
add_subdirectory(src/env)
add_subdirectory(src/http)
add_subdirectory(src/io)
add_subdirectory(src/log)
add_subdirectory(src/net)
add_subdirectory(src/streams)

add_library(sylar ${SYLAR_LIB_TYPE} 
    ${ALL_OBJECT_FILES}
    src/version.cpp
)

force_redefine_file_macro_for_sources(sylar)

set(SYLAR_LIBS
        sylar_app
        sylar_common
        sylar_config
        sylar_daemon
        sylar_env
        sylar_http
        sylar_io
        sylar_log
        sylar_net
        sylar_streams
)

set(SYLAR_THIRDPARTY_LIBS
        dl
        pthread
        yaml-cpp
        pthread
        ${Boost_LIBRARIES}
        ${ZLIB_LIBRARIES}
)

target_compile_options(sylar PRIVATE 
    -fvisibility=hidden
    -fvisibility-inlines-hidden
)

target_link_libraries(
    sylar 
        ${SYLAR_LIBS}
        ${SYLAR_THIRDPARTY_LIBS}
)

target_include_directories(
    sylar PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${SYLAR_INCLUDE_DIR}>
)

# 安装库文件
install(TARGETS sylar
    EXPORT SylarTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 安装头文件
install(
    DIRECTORY ${SYLAR_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.rl"
    PATTERN "version.hpp" EXCLUDE   # <-- 排除这一文件
)

# 安装 yaml-cpp 并加入导出集
install(TARGETS yaml-cpp
    EXPORT SylarTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 生成pkg-config文件
configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/install/sylar.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/sylar.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sylar.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)