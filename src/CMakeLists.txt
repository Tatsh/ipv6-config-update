include_directories(. ${CMAKE_CURRENT_BINARY_DIR}/generated ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL Debug AND (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID
                                                                              STREQUAL Clang))
  set(DEBUG_FLAGS -ggdb -Wall -Werror -Wextra -Wno-unknown-pragmas)
endif()
set(QT_DEPS Qt6::DBus Qt6::Network)

set(LIBIPWATCH_SRCS cidr.cpp cidr.h utils.cpp utils.h)
set(SYSTEMD1_MANAGER_XML manager.xml)

set_source_files_properties(${SYSTEMD1_MANAGER_XML} PROPERTIES CLASSNAME SystemdManager)
qt_add_dbus_interface(LIBIPWATCH_SRCS ${SYSTEMD1_MANAGER_XML} systemd1_interface)

ecm_qt_declare_logging_category(
  LIBIPWATCH_SRCS
  HEADER
  generated/ipwatchdebug.h
  IDENTIFIER
  "LOG_IPV6_CONFIG_UPDATE"
  CATEGORY_NAME
  "sh.tat.ipv6-config-update")

add_library(ipwatch STATIC ${LIBIPWATCH_SRCS}
                           ${CMAKE_CURRENT_BINARY_DIR}/generated/ipwatchdebug.cpp)
target_compile_options(ipwatch PRIVATE ${DEBUG_FLAGS})
target_link_libraries(ipwatch PRIVATE ${QT_DEPS})

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${DEBUG_FLAGS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ipwatch ${QT_DEPS} ${LIBSYSTEMD_LIBRARIES})

if(BUILD_TESTS)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(googletest
                       URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip)
  FetchContent_MakeAvailable(googletest)
  add_executable(${CMAKE_PROJECT_NAME}-tests test.cpp)
  target_compile_options(${CMAKE_PROJECT_NAME}-tests PRIVATE ${DEBUG_FLAGS})
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${CMAKE_PROJECT_NAME}-tests PRIVATE --coverage)
  endif()
  target_link_libraries(
    ${CMAKE_PROJECT_NAME}-tests PRIVATE ipwatch ${QT_DEPS} GTest::gtest_main GTest::gmock gcov
                                        ${LIBSYSTEMD_LIBRARIES})
  include(GoogleTest)
  gtest_discover_tests(${CMAKE_PROJECT_NAME}-tests)
endif()
