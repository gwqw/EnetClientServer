cmake_minimum_required(VERSION 3.2)
project(enet_test)

set(CMAKE_CXX_STANDARD 11)

set(ENET_SERVER_SRC main_server.cpp enet_server.cpp enet_server.h)
set(ENET_CLIENT_SRC main_client.cpp enet_client.cpp enet_client.h)

add_executable(enet_server ${ENET_SERVER_SRC})
add_executable(enet_client ${ENET_CLIENT_SRC})

# enet library
add_subdirectory(enet)

# include dirs
target_include_directories(enet_server PRIVATE enet/include)
target_include_directories(enet_client PRIVATE enet/include)

# linking
target_link_libraries(enet_server enet)
target_link_libraries(enet_client enet)