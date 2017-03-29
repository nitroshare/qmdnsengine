configure_file(qmdnsengine_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengine_export.h")

set(HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengine_export.h"
    include/qmdnsengine/mdns.h
)

set(SRC
    mdns.cpp
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
    RUNTIME       DESTINATION bin
    LIBRARY       DESTINATION lib
    PUBLIC_HEADER DESTINATION include/qmdnsengine
)
