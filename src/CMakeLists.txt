set(IPWATCH_SRCS cidr.cpp main.cpp utils.cpp)
include(ECMQtDeclareLoggingCategory)
set(SYSTEMD1_MANAGER_XML manager.xml)
set_source_files_properties(${SYSTEMD1_MANAGER_XML} PROPERTIES CLASSNAME SystemdManager)
qt_add_dbus_interface(IPWATCH_SRCS ${SYSTEMD1_MANAGER_XML} systemd1_interface)
ecm_qt_declare_logging_category(
  IPWATCH_SRCS
  HEADER
  generated/ipv6configdebug.h
  IDENTIFIER
  "LOG_IPV6_CONFIG_UPDATE"
  CATEGORY_NAME
  "sh.tat.ipv6-config-update")

add_definitions(
  -DQT_NO_CAST_FROM_ASCII
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_STRICT_ITERATORS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_USE_QSTRINGBUILDER)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    add_compile_options(-ggdb -Wall -Werror -Wextra)
  endif()
endif()
include_directories(src ${CMAKE_CURRENT_BINARY_DIR}/generated ${CMAKE_CURRENT_BINARY_DIR})
add_executable(ipv6-config-update ${IPWATCH_SRCS}
                                  ${CMAKE_CURRENT_BINARY_DIR}/generated/ipv6configdebug.cpp)
target_link_libraries(ipv6-config-update Qt5::DBus Qt5::Network ${LIBSYSTEMD_LIBRARIES})

install(TARGETS ipv6-config-update)

include(CPack)
