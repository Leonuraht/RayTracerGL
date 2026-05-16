cmake_minimum_required(VERSION 3.25)
project(app VERSION 1.0 LANGUAGES C CXX) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)
if(NOT MSVC)
    target_compile_options(glad PRIVATE -w)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS 
    "src/*.cpp" 
    "src/*.c"
)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glad         
    glfw        
    glm::glm
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
