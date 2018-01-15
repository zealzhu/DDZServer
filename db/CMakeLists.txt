aux_source_directory(. DB_SOURCES)
add_library(db ${DB_SOURCES})
target_link_libraries(db mysqlcppconn-static)
