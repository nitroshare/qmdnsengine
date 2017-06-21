configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

add_custom_target(doc ALL
    "${DOXYGEN_EXECUTABLE}" \"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile\"
)
