cmake_minimum_required(VERSION 3.28)
project(VeyraEngine LANGUAGES C CXX)

include(CTest)
option(VEYRA_BUILD_EDITOR "Build the Qt desktop editor" ON)
option(VEYRA_BUILD_RUNTIME "Build the Qt-free game runtime" ON)

if(NOT VEYRA_BUILD_EDITOR AND NOT VEYRA_BUILD_RUNTIME)
  message(FATAL_ERROR "Enable at least one of VEYRA_BUILD_EDITOR or VEYRA_BUILD_RUNTIME.")
endif()

if(VEYRA_BUILD_EDITOR)
  add_subdirectory(editor)
endif()

if(VEYRA_BUILD_RUNTIME)
  add_subdirectory(runtime)
endif()
