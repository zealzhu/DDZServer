cmake_minimum_required(VERSION 2.8)
project(game-server)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

message(STATUS "CMake version: " ${CMAKE_VERSION})
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.2")
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(STATUS "Checking compiler flags for C++11 support.")

    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

    if(COMPILER_SUPPORTS_CXX11)
        message(STATUS "C++11 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUD "C++0X is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        endif()
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/spdlog/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/msg)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/db/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/db/lib)

# libmsg
add_subdirectory(msg)

# libprotocol
add_subdirectory(protocol)

# libsocket
add_subdirectory(socket)

# libmodule
add_subdirectory(module)

# libtools
add_subdirectory(tools)

# libdb
add_subdirectory(db)

add_executable(server Server.cc)

target_link_libraries(server tools socket msg module db)
target_link_libraries(server mysqlcppconn)
