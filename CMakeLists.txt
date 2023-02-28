cmake_minimum_required(VERSION 3.13)
project(personal_learning)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}_hello_world_bin ${CMAKE_CURRENT_SOURCE_DIR}/src/hello_world.cpp)

add_executable(${PROJECT_NAME}_hello_world_window_bin ${CMAKE_CURRENT_SOURCE_DIR}/src/hello_world_window.cpp)
target_link_libraries(${PROJECT_NAME}_hello_world_window_bin sfml-graphics)



add_executable(${PROJECT_NAME}_pong_bin ${CMAKE_CURRENT_SOURCE_DIR}/src/pong.cpp)
target_link_libraries(${PROJECT_NAME}_pong_bin sfml-graphics)
