cmake_minimum_required(VERSION 3.7)

project(BasicVulkan++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${CMAKE_PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
add_executable(
    ${CMAKE_PROJECT_NAME}
    ${SOURCE_DIR}/Main.cpp
    ${SOURCE_DIR}/VulkanHandler.cpp
    ${SOURCE_DIR}/WindowHandler.cpp
    )

target_link_libraries(${CMAKE_PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
