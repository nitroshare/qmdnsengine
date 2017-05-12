set(SRC
    browser.cpp
    mainwindow.cpp
)

add_executable(browser WIN32 ${SRC})

target_link_libraries(browser qmdnsengine Qt5::Widgets)
