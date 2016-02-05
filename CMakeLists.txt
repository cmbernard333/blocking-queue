cmake_minimum_required(VERSION 2.8)
project(blocking_queue)

set(SRC_FILES ${SRC_FILES} bus_blocking_queue.c)
add_library(bus_blocking_queue STATIC ${SRC_FILES})

include_directories("simple-thread/include")

add_dependencies(blocking_queue simple_thread)
