include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../opt/inih/cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../opt/spdlog/include)

aux_source_directory(. GAME_TOOLS_SOURCE)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../opt/inih/cpp GAME_TOOLS_SOURCE)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../opt/inih GAME_TOOLS_SOURCE)

add_library(tools ${GAME_TOOLS_SOURCE})
