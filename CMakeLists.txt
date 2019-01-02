cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project (chunklands)

set(CHUNKLANDS_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(CHUNKLANDS_DEPS_DIR "${CMAKE_SOURCE_DIR}/deps")
set(CHUNKLANDS_DEPS_DIR_GLFW "${CHUNKLANDS_DEPS_DIR}/glfw")

file(GLOB SOURCE_FILES "${CHUNKLANDS_SRC_DIR}/*.cc" "${CHUNKLANDS_DEPS_DIR_GLFW}/src/glfw/deps/glad.c")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# OpenGL
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

# cmake-js
include_directories(${CMAKE_JS_INC})
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

# NAPI
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# GLFW 3
if (NOT EXISTS "${CHUNKLANDS_DEPS_DIR_GLFW}")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

  include(ExternalProject)
  ExternalProject_Add(glfw
    PREFIX            "${CHUNKLANDS_DEPS_DIR_GLFW}"
    GIT_REPOSITORY    https://github.com/glfw/glfw.git
    GIT_TAG           3.2.1
  )

  add_dependencies(${PROJECT_NAME} glfw)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CHUNKLANDS_DEPS_DIR_GLFW}/src/glfw/deps")

add_subdirectory("${CHUNKLANDS_DEPS_DIR_GLFW}/src/glfw")
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})