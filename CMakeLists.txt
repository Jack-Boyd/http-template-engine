cmake_minimum_required(VERSION 3.10)
project(TCPServer)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include headers
include_directories(include)

# Build executable
add_executable(server 
    src/server.c 
    src/socket_utils.c 
    src/html_serve.c 
    src/request_handler.c 
    src/template_utils.c)
