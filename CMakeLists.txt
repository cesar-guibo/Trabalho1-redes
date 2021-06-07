cmake_minimum_required(VERSION 3.14)

project(TicTacToe VERSION 0.1.0 LANGUAGES C CXX)

add_definitions("-g -Wall")

set(CMAKE_CXX_STANDARD 17)

add_library(
    tictactoe
    src/tictactoe/game.cpp
    src/tictactoe/game.hpp
    src/tictactoe/server.cpp
	src/tictactoe/server.hpp
	src/tictactoe/argparse.cpp
	src/tictactoe/argparse.hpp
    src/tictactoe/serializable.hpp
    src/tictactoe/serializable.cpp
    src/tictactoe/game_messages.hpp
    src/tictactoe/game_messages.cpp
    src/tictactoe/socket_address.hpp
    src/tictactoe/socket_address.cpp
    src/tictactoe/client.hpp
    src/tictactoe/client.cpp
)
    
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tictactoe PUBLIC Threads::Threads)

add_executable(
    server
    src/server.cpp
)

target_link_libraries(
    server
    PRIVATE
    tictactoe
)

add_executable(
    client
    src/client.cpp
)

target_link_libraries(
    client
    PRIVATE
    tictactoe
)
