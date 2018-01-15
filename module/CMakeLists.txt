aux_source_directory(. GAME_MODULE_SOURCE)
aux_source_directory(./user GAME_MODULE_SOURCE)

add_library(module ${GAME_MODULE_SOURCE})
