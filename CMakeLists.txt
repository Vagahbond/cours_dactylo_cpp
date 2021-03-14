cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)



project(cpp_spellcheck
    VERSION "0.0.1"
    DESCRIPTION "C++ courses"
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_libraries(stdc++fs)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)


find_package(Threads REQUIRED)

if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET cpp_spellcheck PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET cpp_spellcheck PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()



if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(cpp_spellcheck "${CMAKE_THREAD_LIBS_INIT}")
endif()
