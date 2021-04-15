set(HEADERS
    config.h
    error.h
    literal.h
    reader.h
    type.h
    utf.h
    utils.h
    value.h
    value.inl
    writer.h
    json.h
)

include(GNUInstallDirs)

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/include
)