aux_source_directory(. GAME_SOCKET_SOURCE)
add_library(socket ${GAME_SOCKET_SOURCE})
target_link_libraries(socket protocol)
