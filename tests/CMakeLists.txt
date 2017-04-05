set(TESTS
    TestDns
)

foreach(_test ${TESTS})
    add_executable(${_test} ${_test}.cpp)
    set_target_properties(${_test} PROPERTIES
        CXX_STANDARD 11
    )
    target_include_directories(${_test} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${_test} qmdnsengine Qt5::Test)
    add_test(NAME ${_test}
        COMMAND ${_test}
    )
endforeach()
