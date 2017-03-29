cmake_minimum_required(VERSION 3.2.0)
project(qmdnsengine)

set(PROJECT_NAME "QMdnsEngine")
set(PROJECT_DESCRIPTION "Multicast DNS library for Qt applications")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_URL "https://github.com/nitroshare/qmdnsengine")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

find_package(Qt5Network 5.4 REQUIRED)

set(CMAKE_AUTOMOC ON)

add_subdirectory(src)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR $PROJECT_VERSION_MAJOR)
set(CPACK_PACKAGE_VERSION_MINOR $PROJECT_VERSION_MINOR)
set(CPACK_PACKAGE_VERSION_PATCH $PROJECT_VERSION_PATCH)
