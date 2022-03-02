cmake_minimum_required(VERSION 3.16)
project(pbtools C)
add_library(pbtools src/pbtools.c)
target_include_directories(pbtools PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/include")
install(TARGETS pbtools DESTINATION lib)
install(FILES include/pbtools.h DESTINATION include)
