set(HEADERS
    include/qmdnsengine/bitmap.h
    include/qmdnsengine/dns.h
    include/qmdnsengine/mdns.h
    include/qmdnsengine/message.h
    include/qmdnsengine/qmdnsengine_export.h
    include/qmdnsengine/query.h
    include/qmdnsengine/record.h
    include/qmdnsengine/server.h
)

set(SRC
    src/bitmap.cpp
    src/dns.cpp
    src/mdns.cpp
    src/message.cpp
    src/query.cpp
    src/record.cpp
    src/server.cpp
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

add_library(qmdnsengine SHARED ${HEADERS} ${SRC})

set_target_properties(qmdnsengine PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QMDNSENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION       ${PROJECT_VERSION}
    SOVERSION     ${PROJECT_VERSION_MAJOR}
)

target_include_directories(qmdnsengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(qmdnsengine Qt5::Network)

install(TARGETS qmdnsengine
    EXPORT        qmdnsengine-export
    RUNTIME       DESTINATION bin
    LIBRARY       DESTINATION lib
    ARCHIVE       DESTINATION lib
    PUBLIC_HEADER DESTINATION include/qmdnsengine
)

install(EXPORT qmdnsengine-export
    DESTINATION lib/cmake/qmdnsengine
)
