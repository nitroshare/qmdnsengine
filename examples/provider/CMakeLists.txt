set(SRC
    mainwindow.cpp
    provider.cpp
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(provider ${SRC})
else()
    add_executable(provider WIN32 ${SRC})
endif()

target_link_libraries(provider PRIVATE qmdnsengine Qt${QT_VERSION_MAJOR}::Widgets)

install(TARGETS provider
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    LIBRARY DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
