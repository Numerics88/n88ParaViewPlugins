cmake_minimum_required(VERSION 3.12)

project(ParaViewPlugins)

message(STATUS ">>> Let's get started on the CMake process for ParaView plugins.")

find_package (ParaView REQUIRED)
include_directories(${ParaView_DIR})

if (UNIX AND NOT APPLE)
  SET(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib/paraview-5.13")
endif()

# Macro to add a cmake variable to select/deselect the build
# of this plugin.
#
macro (paraview_build_optional_plugin name comment subdirectory default)
  option (PARAVIEW_BUILD_PLUGIN_${name} "Build ${comment}" ${default})
  if (PARAVIEW_BUILD_PLUGIN_${name})
    message (STATUS "Plugin: ${comment} enabled")
    add_subdirectory ("${subdirectory}")
  else (PARAVIEW_BUILD_PLUGIN_${name})
    message (STATUS "Plugin: ${comment} disabled")
  endif (PARAVIEW_BUILD_PLUGIN_${name})
endmacro (paraview_build_optional_plugin)

paraview_build_optional_plugin(
    AIMReader
    "AIM Reader"
    AIMReader
    ON)
paraview_build_optional_plugin(
    N88ModelReader
    "N88 Model Reader"
    N88ModelReader
    ON)
paraview_build_optional_plugin(
    ImageGaussianSmooth
    "Image Gaussian Smooth"
    ImageGaussianSmooth
    ON)
