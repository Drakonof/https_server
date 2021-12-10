cmake_minimum_required(VERSION 3.0)

project(https)

include_directories(inc/)


add_executable(https 
               src/main.cpp
               src/https_server.cpp
               src/http_request.cpp)
 
target_link_libraries(https PUBLIC ssl crypto stdc++fs)

install(TARGETS https RUNTIME DESTINATION bin)
