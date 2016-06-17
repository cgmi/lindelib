cmake_minimum_required (VERSION 3.0)

# create project
project (lindeLib)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

if(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++14 -Wall -Werror -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable")
endif()

# if it is a debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

# add my headers to project
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/ShaderStorageBuffer.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/linde.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/FrameBufferObject.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/FrameBufferObjectMultisample.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Diffusion.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Stopwatch.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Interpolation.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/VertexBufferObject.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Texture.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/TextureMultisample.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/TensorField.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Color.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/GLWindow.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Morphology.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/TextRenderer.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/File.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Convolution.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Shader.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/lindeOpenGL.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/MultiGridDiffusion.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Segmentation.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/StructureExtraction.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/Histogram.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/PaintComposer.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/ResourceHandler.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/KubelkaMunk.h)
list(APPEND lindeLib_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/linde/GLContext.h)

# add my sources to project
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Interpolation.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/File.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Diffusion.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Texture.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/TextureMultisample.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Stopwatch.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/VertexBufferObject.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/GLWindow.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/ShaderStorageBuffer.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Shader.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/FrameBufferObject.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/FrameBufferObjectMultisample.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/TensorField.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/TextRenderer.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/lindeOpenGL.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/MultiGridDiffusion.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Segmentation.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Convolution.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/StructureExtraction.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/PaintComposer.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/Histogram.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/ResourceHandler.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/KubelkaMunk.cpp)
list(APPEND lindeLib_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/GLContext.cpp)

# OpenGL
find_package(OpenGL REQUIRED)
add_definitions(-USE_OPEN_GL)
	
# glew
if (UNIX)
    set(GLEW_LIBRARY libGLEW.so)
else()
    set(GLEW_INCLUDE_DIR "$ENV{GLEW_HOME}/include")
    find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATHS "$ENV{GLEW_HOME}/lib")
    set(GLEW_LIBRARY ${GLEW_LIBRARY})
    message(STATUS "GLEW libs:\t${GLEW_LIBRARY}")
endif(UNIX)
	
# GLFW	
if (WIN32)
	set(GLFW_INCLUDE_DIR "$ENV{GLFW_HOME}/include")
	find_library(GLFW_LIBRARY NAMES glfw3dll glfw glfw3 libglfw libglfw3 PATHS "$ENV{GLFW_HOME}/lib")
	set(GLFW_LIBS ${GLFW_LIBRARY})
	message(STATUS "GLFW libs:\t${GLFW_LIBS}")
else()
find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    set (GLFW_LIBS ${GLFW_STATIC_LIBRARIES})
    list(APPEND GLFW_LIBS libXxf86vm.so)
    list(APPEND GLFW_LIBS libGLU.so)
    list(APPEND GLFW_LIBS libX11.so)
    list(APPEND GLFW_LIBS libXrandr.so)
    list(APPEND GLFW_LIBS libpthread.so)
    list(APPEND GLFW_LIBS libXi.so)
endif()

# Freetype
if (UNIX)
	find_package(Freetype REQUIRED)	
	message(STATUS "Freetype libs:\t${FREETYPE_LIBRARIES}")
else()
	set(FREETYPE_INCLUDE_DIRS "$ENV{FREETYPE_DIR}/include/freetype2")
	find_library(FREETYPE_LIBRARIES NAMES freetype.lib PATHS "$ENV{FREETYPE_DIR}/lib")
	message(STATUS "Freetype libs:\t${FREETYPE_LIBRARIES}")
endif()

# OpenCV
if(EXISTS "$ENV{OpenCV3_DIR}")
set(OpenCV_DIR $ENV{OpenCV3_DIR})
endif(EXISTS "$ENV{OpenCV3_DIR}")
find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)
message(STATUS "OpenCV libs:\t${OpenCV_LIBS}")
message(STATUS "OpenCV version:\t${OpenCV_VERSION}")

# GLM
find_package(glm QUIET)
#if(NOT GLM_FOUND)
#    message(STATUS "Haven't found glm, trying GLM")
#    find_package(GLM REQUIRED)
#endif(NOT GLM_FOUND)

# libTIFF
find_package(TIFF)
if(TIFF_LIBRARIES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_TIFF_STORAGE") 
else()
set(TIFF_INCLUDE_DIR "")
set(TIFF_LIBRARIES "")
endif()

# Qt
# Find includes in corresponding build directories
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
# Find the Qt5Core library
#find_package(Qt5Core 5.4 REQUIRED)
#find_package(Qt5Network 5.4 REQUIRED)



include_directories(
		${CMAKE_CURRENT_LIST_DIR}/include/
                    ${OpenCV_INCLUDE_DIRS}
                    ${GLEW_INCLUDE_DIR}
                    ${GLFW_INCLUDE_DIR}
                    ${FREETYPE_INCLUDE_DIRS}
                    ${OPENGL_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIRS}
                    ${TIFF_INCLUDE_DIR}
                    )

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
                       ${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders/lindeLibShaders)
elseif(WIN32)
#convert slashes to backslashes in windows
file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} CMAKE_CURRENT_BINARY_DIR_WIN)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_LIST_DIR} CMAKE_CURRENT_LIST_DIR_WIN)
# create file junction
execute_process(COMMAND cmd /c mklink /j ${CMAKE_CURRENT_BINARY_DIR_WIN}\\shaders\\lindeLibShaders ${CMAKE_CURRENT_LIST_DIR_WIN}\\shaders)
endif()

target_link_libraries(
			lindeLib
			${OPENGL_LIBRARIES}
			${FREETYPE_LIBRARIES}
            		${GLEW_LIBRARY}
			${EXTRA_LIBS}
            		${GLFW_LIBS}
			${OpenCV_LIBS}
                        ${TIFF_LIBRARIES}
                        #Qt5::Core
                        #Qt5::Network
                        )

