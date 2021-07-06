cmake_minimum_required(VERSION 3.8)
project(AIMReader)

find_package(ParaView REQUIRED)

add_definitions ("-DN88_BUILD_AS_PARAVIEW_PLUGIN")
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

option(N88_BUILD_APPLE "Build plugin for MacOS" OFF)

if (N88_BUILD_APPLE)
  # Requires n88util
  find_package (n88util 2.0 REQUIRED)
  include_directories (${N88UTIL_INCLUDE_DIRS})
  
  # Requires AimIO
  find_package (AimIO 1.0 REQUIRED)
  include_directories (${AIMIO_INCLUDE_DIRS})
   
  # Requires Boost
  find_package (Boost 1.61.0 COMPONENTS filesystem system REQUIRED)
  include_directories (${Boost_INCLUDE_DIR})

else()
	include(ExternalProject)
	
	# Set location for built projects
	set(stage_location "${CMAKE_CURRENT_BINARY_DIR}/stage")
	
	set(project_cmake_params)
	list (APPEND project_cmake_params -DBUILD_SHARED_LIBS:BOOL=ON)
	list (APPEND project_cmake_params -DCMAKE_INSTALL_PREFIX:PATH=${stage_location})
	list (APPEND project_cmake_params -DBoost_INCLUDE_DIR:PATH=${Boost_DIR}/../../../include)
	list (APPEND project_cmake_params -DBoost_DIR:PATH=${Boost_DIR})
	message(STATUS ">>> Boost_DIR = ${Boost_DIR}")
	set(n88util_cmake_params)
	
	set(aimio_cmake_params)
	list (APPEND aimio_cmake_params -DN88_BUILD_AIX:BOOL=OFF)
	
	# n88util
	set (n88util_soversion 2.0)
	ExternalProject_Add (n88util
	  GIT_REPOSITORY "https://github.com/Numerics88/n88util.git"
	  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
	  PREFIX n88util
	  DEPENDS ${boost_target}
	  INSTALL_DIR ${stage_location}
		CMAKE_ARGS ${project_cmake_params} ${n88util_cmake_params}
	)
	# set (n88util_cmake_params
	#   -Dn88util_DIR:PATH=${stage_location}/lib/cmake/n88util-${n88util_soversion}
	#   CACHE INTERNAL "cmake parameters for using n88util")
  # 
	# ExternalProject_Get_Property(n88util source_dir)
	# include_directories(${source_dir}/include/)
	
	# AimIO
	set (aimio_soversion 1.0)
	ExternalProject_Add (aimio
	  GIT_REPOSITORY "https://github.com/Numerics88/AimIO.git"
	  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
	  PREFIX aimio
	  DEPENDS n88util ${boost_target}
	  INSTALL_DIR ${stage_location}
	  CMAKE_ARGS ${project_cmake_params} ${aimio_cmake_params}
	  )
	# set (aimio_cmake_params
	#   -DAimIO_DIR:PATH=${stage_location}/lib/cmake/AimIO-${aimio_soversion}
	#   CACHE INTERNAL "cmake parameters for using aimio")
  # 
	# add_definitions ("-DAimIO_DIR:PATH=${stage_location}/lib/cmake/AimIO-${aimio_soversion}")

	ExternalProject_Get_Property(aimio install_dir)
	include_directories(${install_dir}/include/)

#	-- Installing: /home/buildslave/misc/code/plugin/build/stage/lib/libAimIO.so
#	-- Installing: /home/buildslave/misc/code/plugin/build/stage/include/AimIO
#	-- Installing: /home/buildslave/misc/code/plugin/build/stage/include/AimIO/AimIO.h
	
endif()

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
