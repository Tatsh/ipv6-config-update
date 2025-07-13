cmake_minimum_required(VERSION 3.31)
project(ipv6-config-update VERSION 0.0.1)

find_package(ECM NO_MODULE REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)
include(FindPkgConfig)
include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_TESTS "Build tests." OFF)

if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_OUT} @ONLY)
  add_custom_target(
    docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION})
endif()

find_package(
  Qt6
  COMPONENTS DBus Network
  REQUIRED CONFIG)
set_package_properties(Qt6DBus PROPERTIES TYPE REQUIRED)
set_package_properties(Qt6Network PROPERTIES TYPE REQUIRED)
pkg_check_modules(LIBSYSTEMD libsystemd REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(
  -DQT_NO_CAST_FROM_ASCII
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_STRICT_ITERATORS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_USE_QSTRINGBUILDER
  -DVERSION=\"${CMAKE_PROJECT_VERSION}\")

add_subdirectory(src)

install(TARGETS ${CMAKE_PROJECT_NAME})
install(FILES man/${CMAKE_PROJECT_NAME}.1 TYPE MAN)
include(CPack)
