cmake_minimum_required(VERSION 3.8)
project(AIMReader)

# Requires n88util
find_package (n88util 2.0 REQUIRED)
include_directories (${N88UTIL_INCLUDE_DIRS})
#message(STATUS "*** N88UTIL_INCLUDE_DIRS is set to ${N88UTIL_INCLUDE_DIRS}")

# Requires AimIO
find_package (AimIO 1.0 REQUIRED)
include_directories (${AIMIO_INCLUDE_DIRS})
#message(STATUS "*** AIMIO_INCLUDE_DIRS is set to ${AIMIO_INCLUDE_DIRS}")

# Requires Boost
find_package (Boost 1.61.0 COMPONENTS filesystem system REQUIRED)
include_directories (${Boost_INCLUDE_DIR})
#message(STATUS "*** Boost_INCLUDE_DIR is set to ${Boost_INCLUDE_DIR}")

add_definitions ("-DN88_BUILD_AS_PARAVIEW_PLUGIN")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set("_paraview_plugin_default_${CMAKE_PROJECT_NAME}" ON)
paraview_plugin_scan(
  PLUGIN_FILES      "${CMAKE_CURRENT_SOURCE_DIR}/Plugin/paraview.plugin"
  PROVIDES_PLUGINS  plugins
	ENABLE_BY_DEFAULT	ON
  REQUIRES_MODULES  required_modules)

foreach (module IN LISTS required_modules)
  if (NOT TARGET "${module}")
    message(#FATAL_ERROR
      "Skipping example ${CMAKE_PROJECT_NAME}: Missing required module: "
      "${module}")
    return ()
  endif ()
endforeach ()

paraview_plugin_build(
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  PLUGINS ${plugins})
