cmake_minimum_required(VERSION 3.12)

project(ParaViewPlugins)

message(STATUS ">>> Let's get started on the CMake process for ParaView plugins.")

find_package (ParaView REQUIRED)

# This is a good RPATH for plugins that are installed like this:
#    lib/paraview-X.Y/plugins/N88ModelReader/N88ModelReader.so
if (UNIX AND NOT APPLE)
  SET(CMAKE_INSTALL_RPATH "$ORIGIN/../../../../lib:$ORIGIN:$ORIGIN/../../../")
  SET(CMAKE_BUILD_RPATH "${CMAKE_INSTALL_RPATH}")
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
