add_executable(CHAT_1  echo_server.cpp)
target_link_libraries(CHAT_1 PUBLIC asio::asio)


add_executable(ECHO_SERVER_1  refactored_echo_server.cpp)
target_link_libraries(ECHO_SERVER_1 PUBLIC asio::asio)


add_executable(CB_1  callback_wrapper.cpp)
target_link_libraries(CB_1 PUBLIC asio::asio)


add_executable(REF_COUNTER  reference_counted.cpp)
target_link_libraries(REF_COUNTER PUBLIC asio::asio)

add_executable(ACTOR  actor.cpp)
target_link_libraries(ACTOR PUBLIC asio::asio)


add_executable(ECHO_S  blocking_tcp_echo_server.cpp)
add_executable(AECHO_S  async_tcp_echo_server.cpp)
add_executable(ECHO_C  blocking_tcp_echo_client.cpp)
target_link_libraries(ECHO_S PUBLIC asio::asio)
target_link_libraries(AECHO_S PUBLIC asio::asio)
target_link_libraries(ECHO_C PUBLIC asio::asio)
