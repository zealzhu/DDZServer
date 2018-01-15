aux_source_directory(. PROTOCOL_SOURCE)

add_library(protocol ${PROTOCOL_SOURCE})
target_link_libraries(protocol msg)
