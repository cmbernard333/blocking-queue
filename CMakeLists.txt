cmake_minimum_required(VERSION 2.8)
project(blocking_queue)

add_subdirectory(simplethread)

set(SRC_FILES 
	blocking_queue.c)
add_library(blocking_queue STATIC ${SRC_FILES})

include_directories("simple-thread/include")

add_dependencies(blocking_queue simple_thread)
