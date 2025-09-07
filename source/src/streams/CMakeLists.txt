aux_source_directory(. SRC_LIST)

add_library(
    sylar_streams
    OBJECT
    ${SRC_LIST}
)

force_redefine_file_macro_for_sources(sylar_streams)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES}
    $<TARGET_OBJECTS:sylar_streams>
    PARENT_SCOPE
)

install(TARGETS sylar_streams
    EXPORT SylarTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
