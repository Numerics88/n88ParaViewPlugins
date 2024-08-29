cmake_minimum_required(VERSION 3.12)
project(AIMReader)

find_package(ParaView REQUIRED)

add_definitions ("-DN88_BUILD_AS_PARAVIEW_PLUGIN")
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# We used to build the dependencies separately, but it is convenient to be able 
# to build them all in a single command, especially on Linux for paraview-plugin-builder. 
# Only turn N88_BUILD_PACKAGES to ON if neccessary.
option(N88_BUILD_PACKAGES "Build dependencies for the plugins separately." OFF)

if (N88_BUILD_PACKAGES)
  # Requires n88util
  find_package (n88util 2.0 REQUIRED)
  include_directories (${N88UTIL_INCLUDE_DIRS})
  
  # Requires AimIO
  find_package (AimIO 1.0 REQUIRED)
  include_directories (${AIMIO_INCLUDE_DIRS})
   
  # Requires Boost
  find_package (Boost 1.70.0 COMPONENTS filesystem system REQUIRED)
  include_directories (${Boost_INCLUDE_DIR})

else()
	include(ExternalProject)
	
	# Set location for built projects
	set(stage_location "${CMAKE_CURRENT_BINARY_DIR}/stage")
	
	set(project_cmake_params)
	list (APPEND project_cmake_params -DBUILD_SHARED_LIBS:BOOL=OFF)
	if (NOT CMAKE_BUILD_TYPE STREQUAL "")
	  list (APPEND project_cmake_params -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
	endif()
	if (APPLE)
	  list (APPEND project_cmake_params
	    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
	    -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
	    -DCMAKE_OSX_SYSROOT:PATH=${CMAKE_OSX_SYSROOT})
	endif ()
	# need to compile with -std=c++14 to use boost >= 1.75
	list (APPEND project_cmake_params -DCMAKE_CXX_STANDARD:STRING=14)
	list (APPEND project_cmake_params -DCMAKE_INSTALL_PREFIX:PATH=${stage_location})
	list (APPEND project_cmake_params -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC)

	# message(STATUS ">>> Checking the boost build:")
	# find_program(LS ls)
	# if (LS)
	# 	message(STATUS ">>> Boost_DIR: ${LS} -l ${Boost_DIR}")
	# 	execute_process(COMMAND ${LS} -l ${Boost_DIR})
	# endif()
	
	# boost ------------------------------------------------------------------------------------------
	set (boost_version 1.85.0)
	string (REPLACE "." "_" boost_tarfile_version ${boost_version})
	set (extra_bootstrap_commands)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	  set (extra_bootstrap_commands --with-toolset=clang)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	  set (extra_bootstrap_commands --with-toolset=intel)
	endif()
	
	set (extra_build_commands)
	list (APPEND extra_build_commands variant=release)
	list (APPEND extra_build_commands link=static)
	list (APPEND extra_build_commands cxxflags=-fPIC cflags=-fPIC)
	if (CMAKE_OSX_DEPLOYMENT_TARGET)
	  list (APPEND extra_build_commands cflags="-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}"
	                                    cxxflags="-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}"
	                                    linkflags="-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
	endif()

	set (library_build_commands
	  --with-filesystem
	  --with-system
	  --with-thread
	  )

	ExternalProject_Add (boost
	  URL "https://sourceforge.net/projects/boost/files/boost/${boost_version}/boost_${boost_tarfile_version}.tar.bz2"
	  URL_HASH SHA256=7009fe1faa1697476bdc7027703a2badb84e849b7b0baad5086b087b971f8617
	  PREFIX boost
	  DOWNLOAD_DIR ${download_location}
	  INSTALL_DIR ${stage_location}
	  BUILD_IN_SOURCE 1
	  CONFIGURE_COMMAND
	    <SOURCE_DIR>/bootstrap.sh
	                      ${extra_bootstrap_commands}
	  BUILD_COMMAND <SOURCE_DIR>/b2
	                      ${extra_build_commands}
	                      ${library_build_commands}
	  INSTALL_COMMAND <SOURCE_DIR>/b2
	                      ${extra_build_commands}
	                      ${library_build_commands}
	                      install --prefix=<INSTALL_DIR>
	  )
	set(boost_target boost CACHE INTERNAL "Boost target")
	set(release_suffix "")
	set(lib_ext a)
	set(boost_cmake_params
	  "-DBoost_DIR:PATH=${stage_location}/lib/cmake/Boost-${boost_version}"
	  CACHE INTERNAL "cmake parameters for using boost")

	# n88util ----------------------------------------------------------------------------------------
	set(n88util_cmake_params)
	set(n88util_soversion 2.0)
	ExternalProject_Add (n88util
	  GIT_REPOSITORY "https://github.com/Numerics88/n88util.git"
	  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
	  PREFIX n88util
	  DEPENDS ${boost_target}
	  INSTALL_DIR ${stage_location}
	  CMAKE_ARGS ${project_cmake_params} ${n88util_cmake_params} ${boost_cmake_params}
	)
	
	# AimIO ------------------------------------------------------------------------------------------
	set(aimio_cmake_params)
	list (APPEND aimio_cmake_params -DN88_BUILD_AIX:BOOL=OFF)
	set(aimio_soversion 1.0)
	ExternalProject_Add (aimio
	  GIT_REPOSITORY "https://github.com/Numerics88/AimIO.git"
	  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
	  PREFIX aimio
	  DEPENDS n88util ${boost_target}
	  INSTALL_DIR ${stage_location}
	  CMAKE_ARGS ${project_cmake_params} ${aimio_cmake_params} ${boost_cmake_params}
	  )
	# set (aimio_cmake_params
	#   -DAimIO_DIR:PATH=${stage_location}/lib/cmake/AimIO-${aimio_soversion}
	#   CACHE INTERNAL "cmake parameters for using aimio")

	ExternalProject_Get_Property(aimio install_dir)
	include_directories(${install_dir}/include/)
	link_directories(${install_dir}/lib/)

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
    message(STATUS #FATAL_ERROR
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
