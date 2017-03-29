set(HEADERS
    include/qmdnsengine/dns.h
    include/qmdnsengine/mdns.h
    include/qmdnsengine/message.h
    include/qmdnsengine/qmdnsengine_export.h
)

set(SRC
    src/dns.cpp
    src/mdns.cpp
    src/message.cpp
)

add_library(qmdnsengine SHARED ${HEADERS} ${SRC})

set_target_properties(qmdnsengine PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QMDNSENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION       ${VERSION}
    SOVERSION     ${VERSION_MAJOR}
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
    PUBLIC_HEADER DESTINATION include/qmdnsengine
)

install(EXPORT qmdnsengine-export
    DESTINATION lib/cmake/qmdnsengine
)
