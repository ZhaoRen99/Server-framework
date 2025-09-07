aux_source_directory(. SRC_LIST)

add_library(
    sylar_common
    OBJECT
    ${SRC_LIST}
)

force_redefine_file_macro_for_sources(sylar_common)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES}
    $<TARGET_OBJECTS:sylar_common>
    PARENT_SCOPE
)

install(TARGETS sylar_common
    EXPORT SylarTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
