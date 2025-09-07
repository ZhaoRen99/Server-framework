aux_source_directory(. SRC_LIST)

ragelmaker(${PROJECT_SOURCE_DIR}/source/include/sylar/http/http11_parser.rl SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR})
ragelmaker(${PROJECT_SOURCE_DIR}/source/include/sylar/http/httpclient_parser.rl SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR})
ragelmaker(${PROJECT_SOURCE_DIR}/source/include/sylar/http/uri.rl SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    sylar_http
    OBJECT
    ${SRC_LIST}
)

force_redefine_file_macro_for_sources(sylar_http)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES}
    $<TARGET_OBJECTS:sylar_http>
    PARENT_SCOPE
)

install(TARGETS sylar_http
    EXPORT SylarTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
