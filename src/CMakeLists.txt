set(JSONCPP_INCLUDE_DIR ../include)

set(HEADERS
    ${JSONCPP_INCLUDE_DIR}/config.h
    ${JSONCPP_INCLUDE_DIR}/error.h
    ${JSONCPP_INCLUDE_DIR}/literal.h
    ${JSONCPP_INCLUDE_DIR}/reader.h
    ${JSONCPP_INCLUDE_DIR}/type.h
    ${JSONCPP_INCLUDE_DIR}/utf.h
    ${JSONCPP_INCLUDE_DIR}/utils.h
    ${JSONCPP_INCLUDE_DIR}/value.h
    ${JSONCPP_INCLUDE_DIR}/value.inl
    ${JSONCPP_INCLUDE_DIR}/writer.h
    ${JSONCPP_INCLUDE_DIR}/json.h
)

set(SOURCES
    value.cpp
    reader.cpp
    writer.cpp
    literal.cpp
)

function(set_targets lib)
    target_compile_features(${lib} PUBLIC cxx_std_11)
    target_include_directories(
        ${lib} PUBLIC 
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSONCPP_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endfunction(set_targets lib)

# Build static jsoncpp lib
if(BUILD_STATIC_LIB)
    set(STATIC_LIB ${PROJECT_NAME}_static)
    add_library(${STATIC_LIB} STATIC ${HEADERS} ${SOURCES})
    set_targets(${STATIC_LIB})
endif(BUILD_STATIC_LIB)

# Build shared jsoncpp lib
if(BUILD_SHARED_LIB)
    set(SHARED_LIB ${PROJECT_NAME}_shared)
    add_definitions(-DJSON_DLL_BUILD)
    add_library(${SHARED_LIB} SHARED ${HEADERS} ${SOURCES})
    set_targets(${SHARED_LIB})
endif(BUILD_SHARED_LIB)