set(SRC
    browser.cpp
    mainwindow.cpp
    servicemodel.cpp
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(browser ${SRC})
else()
    add_executable(browser WIN32 ${SRC})
endif()

set_target_properties(browser PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(browser PRIVATE qmdnsengine Qt${QT_VERSION_MAJOR}::Widgets)

install(TARGETS browser
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    LIBRARY DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
