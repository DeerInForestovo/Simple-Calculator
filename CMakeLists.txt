add_compile_options(-std=c++11)

project(simbc)

aux_source_directory(. dir)

add_executable(simbc ${dir})