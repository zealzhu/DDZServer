file(GLOB PROTO_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
foreach(PROTO_FILE ${PROTO_INPUT})
    PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})
    message(STATUS "CUR_PROTO_GEN = ${PROTO_HDRS} ${PROTO_SRCS}")

    set(PROTO_GEN
        ${PROTO_GEN}
        ${PROTO_SRCS})
endforeach(PROTO_FILE ${PROTO_INPUT})

add_library(msg STATIC ${PROTO_GEN} MessageManager.cpp Semaphore.cpp)
target_link_libraries(msg ${PROTOBUF_LIBRARIES} pthread)
