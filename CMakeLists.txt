cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

include(ExternalProject)

project (chunklands)

set(SRC "${CMAKE_SOURCE_DIR}/src")

set(CHUNKLANDS_LIBRARIES "")
set(CHUNKLANDS_INCLUDE "${CMAKE_SOURCE_DIR}/src")

# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${OPENGL_INCLUDE_DIR}")
set(CHUNKLANDS_LIBRARIES "${CHUNKLANDS_LIBRARIES}" "${OPENGL_gl_LIBRARY}")

# node
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${CMAKE_SOURCE_DIR}/deps/node/src")

# NAPI
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")

# GLFW 3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/deps/glfw")
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${CMAKE_SOURCE_DIR}/deps/glfw/deps") # glad/glad.h
set(CHUNKLANDS_LIBRARIES "${CHUNKLANDS_LIBRARIES}" glfw)

# GLM
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/deps/glm")
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${CMAKE_SOURCE_DIR}/deps/glm")
set(CHUNKLANDS_LIBRARIES "${CHUNKLANDS_LIBRARIES}" glm_static)

# Boost
file(GLOB BOOST_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/deps/boost_*/include")
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" ${BOOST_INCLUDE_DIRS})

# STB
set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${CMAKE_SOURCE_DIR}/deps/stb")

# Backtrace
find_package(Backtrace)
if (Backtrace_FOUND)
  set(CHUNKLANDS_LIBRARIES "${CHUNKLANDS_LIBRARIES}" "backtrace")
  set(CHUNKLANDS_INCLUDE "${CHUNKLANDS_INCLUDE}" "${Backtrace_INCLUDE_DIRS}")
endif()


message("LIBRARIES:     ${CHUNKLANDS_LIBRARIES}")
message("INCLUDE:       ${CHUNKLANDS_INCLUDE}")
message("SOURCE_FILES:  ${SOURCE_FILES}")



# BUILD GENERIC
file(GLOB_RECURSE CHUNKLANDS_GENERIC_SOURCE_FILES
  "${SRC}/chunklands/*.cc"
  "${SRC}/chunklands/**/*.cc"
  "${CMAKE_SOURCE_DIR}/deps/glfw/deps/glad.c"
  "${CMAKE_SOURCE_DIR}/deps/boost_chrono/src/*.cpp"
)
add_library               (chunklands_generic SHARED ${CHUNKLANDS_GENERIC_SOURCE_FILES})
target_include_directories(chunklands_generic SYSTEM PRIVATE "${CHUNKLANDS_INCLUDE}")
target_link_libraries     (chunklands_generic "${CHUNKLANDS_LIBRARIES}")
target_compile_options    (chunklands_generic PRIVATE -Werror -Wall -Wextra)


# BUILD NAPI MODULE
file(GLOB_RECURSE CHUNKLANDS_SOURCE_FILES
  "${SRC}/chunklands_node/*.cc"
  "${SRC}/chunklands_node/**/*.cc"
)
add_library               (chunklands SHARED ${CHUNKLANDS_SOURCE_FILES})
set_target_properties     (chunklands PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(chunklands SYSTEM PRIVATE "${CHUNKLANDS_INCLUDE}")
target_link_libraries     (chunklands chunklands_generic)
target_compile_options    (chunklands PRIVATE -Werror -Wall -Wextra)


# # TEST
# enable_testing()
# 
# # Download and unpack googletest at configure time
# configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
# if(result)
#   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
# endif()
# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
# if(result)
#   message(FATAL_ERROR "Build step for googletest failed: ${result}")
# endif()
# 
# # Prevent overriding the parent project's compiler/linker
# # settings on Windows
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# 
# # Add googletest directly to our build. This defines
# # the gtest and gtest_main targets.
# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
#                  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
#                  EXCLUDE_FROM_ALL)
# 
# # The gtest/gtest_main targets carry header search path
# # dependencies automatically when using CMake 2.8.11 or
# # later. Otherwise we have to add them here ourselves.
# if (CMAKE_VERSION VERSION_LESS 2.8.11)
#   include_directories("${gtest_SOURCE_DIR}/include")
# endif()
# 
# 
# file(GLOB_RECURSE TEST_SOURCE_FILES
#   "${CMAKE_SOURCE_DIR}/src/*.cxx"
#   "${CMAKE_SOURCE_DIR}/src/**/*.cxx"
# )
# 
# # Now simply link against gtest or gtest_main as needed. Eg
# add_executable(chunklands_test ${TEST_SOURCE_FILES})
# target_include_directories(chunklands_test SYSTEM PRIVATE "${gtest_SOURCE_DIR}/include" ${CHUNKLANDS_INCLUDE})
# target_link_libraries(chunklands_test gtest_main ${PROJECT_NAME})
# 
# add_test(NAME chunklands_test COMMAND chunklands_test)