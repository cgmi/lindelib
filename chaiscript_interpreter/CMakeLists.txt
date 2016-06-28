cmake_minimum_required (VERSION 3.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)

set (project_name chaiscript_interpreter)

project (${project_name})


list(APPEND HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/Chai.h)
list(APPEND HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/Chai.cpp)

list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Qt
# Find includes in corresponding build directories
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
#set(CMAKE_AUTOMOC ON)
# Find the Qt5Core library
#find_package(Qt5Core)
#find_package(Qt5Widgets)
#find_package(Qt5Network)

file(GLOB SHADER_FILES ${CMAKE_CURRENT_LIST_DIR}/src/shaders/*[.glsl|.frag|.vert])
source_group(shaders FILES ${SHADER_FILES})

# executable
add_executable(${project_name} ${HEADERS} ${SOURCES} ${SHADER_FILES})

# GLSL shaders copy
if(UNIX)
add_custom_command(TARGET ${project_name} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                       ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders/applicationShaders)
elseif(WIN32)
#convert slashes to backslashes in windows
file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} CMAKE_CURRENT_BINARY_DIR_WIN)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_LIST_DIR} CMAKE_CURRENT_LIST_DIR_WIN)
# create file junction
execute_process(COMMAND cmd /c mklink /j ${CMAKE_CURRENT_BINARY_DIR_WIN}\\shaders\\applicationShaders ${CMAKE_CURRENT_LIST_DIR_WIN}\\src\\shaders)
endif()	

target_link_libraries(${project_name}
    lindeLib
   # Qt5::Core
   # Qt5::Network
   # Qt5::Widgets
)
