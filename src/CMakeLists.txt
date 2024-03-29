configure_file(qmdnsengine_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengine_export.h")

set(HEADERS
    include/qmdnsengine/abstractserver.h
    include/qmdnsengine/bitmap.h
    include/qmdnsengine/browser.h
    include/qmdnsengine/cache.h
    include/qmdnsengine/dns.h
    include/qmdnsengine/hostname.h
    include/qmdnsengine/mdns.h
    include/qmdnsengine/message.h
    include/qmdnsengine/prober.h
    include/qmdnsengine/provider.h
    include/qmdnsengine/query.h
    include/qmdnsengine/record.h
    include/qmdnsengine/resolver.h
    include/qmdnsengine/server.h
    include/qmdnsengine/service.h
    "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengine_export.h"
)

set(SRC
    src/abstractserver.cpp
    src/bitmap.cpp
    src/browser.cpp
    src/cache.cpp
    src/dns.cpp
    src/hostname.cpp
    src/mdns.cpp
    src/message.cpp
    src/prober.cpp
    src/provider.cpp
    src/query.cpp
    src/record.cpp
    src/resolver.cpp
    src/server.cpp
    src/service.cpp
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

add_library(qmdnsengine ${HEADERS} ${SRC})

if ( "${QT_VERSION}" VERSION_GREATER_EQUAL "6.6.0" )
    message( STATUS "QMdnsEngine: Enable C++17 for Qt 6.6 and newer" )
    set_target_properties(qmdnsengine PROPERTIES
        CXX_STANDARD          17
    )
else()
	set_target_properties(qmdnsengine PROPERTIES
        CXX_STANDARD          11
    )
endif()

set_target_properties(qmdnsengine PROPERTIES
    CXX_STANDARD_REQUIRED ON
    DEFINE_SYMBOL         QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL         QT_NO_FOREACH
    DEFINE_SYMBOL         QMDNSENGINE_LIBRARY
    PUBLIC_HEADER         "${HEADERS}"
    VERSION               ${PROJECT_VERSION}
    SOVERSION             ${PROJECT_VERSION_MAJOR}
)

target_include_directories(qmdnsengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

target_link_libraries(qmdnsengine Qt${QT_VERSION_MAJOR}::Network)

install(TARGETS qmdnsengine
    EXPORT        qmdnsengine-export
    RUNTIME       DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY       DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE       DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/qmdnsengine"
)

install(EXPORT qmdnsengine-export
    FILE        qmdnsengineConfig.cmake
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qmdnsengine"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/qmdnsengineConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengineConfigVersion.cmake"
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qmdnsengine"
)
