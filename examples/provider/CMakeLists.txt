set(SRC
    mainwindow.cpp
    provider.cpp
)

add_executable(provider WIN32 ${SRC})

target_link_libraries(provider qmdnsengine Qt${QT_VERSION_MAJOR}::Widgets)

install(TARGETS provider
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
