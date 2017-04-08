add_executable(browser WIN32 browser.cpp)

target_link_libraries(browser qmdnsengine Qt5::Widgets)
