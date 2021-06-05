cmake_minimum_required(VERSION 3.14)

project(TicTacToe VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)


add_library(
    tictactoe
    src/tictactoe/game_server.cpp
	src/tictactoe/game_server.hpp
)
    
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tictactoe PUBLIC Threads::Threads)

# target_include_directories(
#     tictactoe
#     PUBLIC lib
# )

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
