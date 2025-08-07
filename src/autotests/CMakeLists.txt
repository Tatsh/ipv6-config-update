include(ECMMarkAsTest)

function(create_test test_name test_srcs)
  ecm_qt_declare_logging_category(
    test_srcs
    HEADER
    generated/ipwatchdebug.h
    IDENTIFIER
    "LOG_IPV6_CONFIG_UPDATE"
    CATEGORY_NAME
    "sh.tat.ipv6-config-update")
  add_executable(${test_name} ${test_srcs})
  target_compile_definitions(${test_name} PRIVATE TESTING)
  target_compile_options(${test_name} PRIVATE ${DEBUG_FLAGS})
  target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
  if(COVERAGE AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${test_name} PRIVATE --coverage)
    target_link_libraries(${test_name} PRIVATE gcov)
  endif()
  target_link_libraries(${test_name} PRIVATE Qt6::Test)
  add_test(NAME ${test_name} COMMAND ${test_name})
  # ecm_mark_as_test(${test_name})
endfunction()

set(cidr_tests_SRCS cidrtest.cpp ../cidr.cpp ../cidr.h ../networkinfo.h)
create_test(cidr-tests "${cidr_tests_SRCS}")
target_link_libraries(cidr-tests PRIVATE Qt6::Network)

set(updater_tests_SRCS updatertest.cpp ../updater.cpp ../updater.h)
create_test(updater-tests "${updater_tests_SRCS}")
