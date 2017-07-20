cmake_minimum_required (VERSION 3.1)

# create project
project (lindeLib)

set (CMAKE_CXX_STANDARD 14)

if(UNIX)
    add_definitions(-Wall -Werror -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable)
endif()

set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

# add headers to project
list(APPEND lindeLib_HEADERS
    ${PROJECT_DIR}/include/linde/ShaderStorageBuffer.h
    ${PROJECT_DIR}/include/linde/linde.h
    ${PROJECT_DIR}/include/linde/FrameBufferObject.h
    ${PROJECT_DIR}/include/linde/FrameBufferObjectMultisample.h
    ${PROJECT_DIR}/include/linde/Diffusion.h
    ${PROJECT_DIR}/include/linde/Stopwatch.h
    ${PROJECT_DIR}/include/linde/Interpolation.h
    ${PROJECT_DIR}/include/linde/VertexBufferObject.h
    ${PROJECT_DIR}/include/linde/VertexArray.h
    ${PROJECT_DIR}/include/linde/Texture.h
    ${PROJECT_DIR}/include/linde/TextureMultisample.h
    ${PROJECT_DIR}/include/linde/TensorField.h
    ${PROJECT_DIR}/include/linde/Color.h
    ${PROJECT_DIR}/include/linde/GLWindow.h
    ${PROJECT_DIR}/include/linde/Morphology.h
    ${PROJECT_DIR}/include/linde/TextRenderer.h
    ${PROJECT_DIR}/include/linde/File.h
    ${PROJECT_DIR}/include/linde/Convolution.h
    ${PROJECT_DIR}/include/linde/Shader.h
    ${PROJECT_DIR}/include/linde/lindeOpenGL.h
    ${PROJECT_DIR}/include/linde/MultiGridDiffusion.h
    ${PROJECT_DIR}/include/linde/Segmentation.h
    ${PROJECT_DIR}/include/linde/StructureExtraction.h
    ${PROJECT_DIR}/include/linde/Histogram.h
    ${PROJECT_DIR}/include/linde/ResourceHandler.h
    ${PROJECT_DIR}/include/linde/KubelkaMunk.h
    ${PROJECT_DIR}/include/linde/GLContext.h
    ${PROJECT_DIR}/include/linde/Thread.h
)

# add sources to project
list(APPEND lindeLib_SOURCES
    ${PROJECT_DIR}/src/Interpolation.cpp
    ${PROJECT_DIR}/src/File.cpp
    ${PROJECT_DIR}/src/Diffusion.cpp
    ${PROJECT_DIR}/src/Texture.cpp
    ${PROJECT_DIR}/src/TextureMultisample.cpp
    ${PROJECT_DIR}/src/Stopwatch.cpp
    ${PROJECT_DIR}/src/VertexBufferObject.cpp
    ${PROJECT_DIR}/src/VertexArray.cpp
    ${PROJECT_DIR}/src/GLWindow.cpp
    ${PROJECT_DIR}/src/ShaderStorageBuffer.cpp
    ${PROJECT_DIR}/src/Shader.cpp
    ${PROJECT_DIR}/src/Color.cpp
    ${PROJECT_DIR}/src/FrameBufferObject.cpp
    ${PROJECT_DIR}/src/FrameBufferObjectMultisample.cpp
    ${PROJECT_DIR}/src/TensorField.cpp
    ${PROJECT_DIR}/src/TextRenderer.cpp
    ${PROJECT_DIR}/src/lindeOpenGL.cpp
    ${PROJECT_DIR}/src/MultiGridDiffusion.cpp
    ${PROJECT_DIR}/src/Segmentation.cpp
    ${PROJECT_DIR}/src/Convolution.cpp
    ${PROJECT_DIR}/src/StructureExtraction.cpp
    ${PROJECT_DIR}/src/Histogram.cpp
    ${PROJECT_DIR}/src/ResourceHandler.cpp
    ${PROJECT_DIR}/src/KubelkaMunk.cpp
    ${PROJECT_DIR}/src/GLContext.cpp
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# OpenGL
find_package(OpenGL REQUIRED)
add_definitions(-USE_OPEN_GL)
include_directories(${OPENGL_INCLUDE_DIRS})
list(APPEND used_libaries  ${OPENGL_LIBRARIES})
	
# GLEW (not needed for Apple)
find_package(GLEW)
if (NOT GLEW_FOUND AND NOT APPLE)
    message (FATAL_ERROR "GLEW not found!")
    return()
elseif(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    list(APPEND used_libaries  ${GLEW_LIBRARIES})
endif ()
	
# GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
set (GLFW_LIBS ${GLFW_STATIC_LIBRARIES})
if (UNIX AND NOT APPLE)
    list(APPEND GLFW_LIBS libXxf86vm.so)
    list(APPEND GLFW_LIBS libGLU.so)
    list(APPEND GLFW_LIBS libX11.so)
    list(APPEND GLFW_LIBS libXrandr.so)
    list(APPEND GLFW_LIBS libpthread.so)
    list(APPEND GLFW_LIBS libXi.so)
endif()

# Freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
list(APPEND used_libaries ${FREETYPE_LIBRARIES})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
list(APPEND used_libaries ${OpenCV_LIBS})

# GLM
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

# libTIFF
find_package(TIFF)
if(TIFF_LIBRARIES)
    list(APPEND used_libaries ${TIFF_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_TIFF_STORAGE") 
else()
    set(TIFF_INCLUDE_DIR "")
    set(TIFF_LIBRARIES "")
endif()

# shaders
file(GLOB lindeLib_SHADER_FILES ${CMAKE_CURRENT_LIST_DIR}/shaders/*[.glsl|.frag|.vert])
source_group(shaders FILES ${lindeLib_SHADER_FILES})

# executable
add_library(lindeLib STATIC ${lindeLib_HEADERS} ${lindeLib_SOURCES} ${lindeLib_SHADER_FILES})

# GLSL shaders link folder
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
if(UNIX)
    add_custom_command(TARGET lindeLib PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_LIST_DIR}/shaders 
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/lindeLibShaders
        )
elseif(WIN32)
    #convert slashes to backslashes in windows
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} CMAKE_CURRENT_BINARY_DIR_WIN)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_LIST_DIR} CMAKE_CURRENT_LIST_DIR_WIN)
    # create file junction
    execute_process(COMMAND cmd /c mklink /j ${CMAKE_CURRENT_BINARY_DIR_WIN}\\shaders\\lindeLibShaders ${CMAKE_CURRENT_LIST_DIR_WIN}\\shaders)
endif()

target_link_libraries(
    lindeLib
    ${GLFW_LIBS}
    ${used_libaries}    
)
