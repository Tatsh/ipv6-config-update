cmake_minimum_required(VERSION 3.31)
project(
  ipv6-config-update
  LANGUAGES CXX
  VERSION 0.1.2
  DESCRIPTION "Update IPv6 CIDR in config files and restart relevant systemd units."
  HOMEPAGE_URL "https://github.com/Tatsh/ipv6-config-update")

include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_DOCS_ONLY "Build documentation only." OFF)
option(BUILD_TESTS "Build tests." OFF)
option(COVERAGE "Enable code coverage." OFF)
option(INSTALL_DOCS "Install documentation." OFF)

if(NOT BUILD_DOCS_ONLY)
  find_package(ECM NO_MODULE REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

  include(ECMQtDeclareLoggingCategory)
  include(FindPkgConfig)

  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)

  find_package(
    Qt6
    COMPONENTS DBus Network
    REQUIRED CONFIG)
  if(BUILD_TESTS)
    find_package(Qt6Test REQUIRED)
  endif()
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
  install(FILES man/${CMAKE_PROJECT_NAME}.1 DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/man/man1)
  install(FILES ${CMAKE_PROJECT_NAME}.service ${CMAKE_PROJECT_NAME}.timer
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system)
endif()

if(BUILD_DOCS OR BUILD_DOCS_ONLY)
  set(DOXYGEN_EXCLUDE_PATTERNS */autotests/*)
  set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tags")
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  set(DOXYGEN_TAGFILES ./.docs/qtcore.tags=http://doc.qt.io/qt-6/)
  find_package(Doxygen REQUIRED COMPONENTS dot)
  doxygen_add_docs(docs README.md src ALL)
  if(INSTALL_DOCS)
    install(
      DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION})
  endif()
endif()

if(NOT BUILD_DOCS_ONLY)
  set(CPACK_PACKAGE_VENDOR Tatsh)
  set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
  set(CPACK_GENERATOR ZIP)
  set(CPACK_THREADS 0)
  set(CPACK_PACKAGE_FILE_NAME
      ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})
  include(CPack)
endif()
