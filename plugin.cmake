include(ExternalProject)

# Set location for built projects
set(stage_location "${CMAKE_CURRENT_BINARY_DIR}/stage")

set(project_cmake_params)
list (APPEND project_cmake_params -DBUILD_SHARED_LIBS:BOOL=ON)

# n88util
set (n88util_soversion 2.0)
ExternalProject_Add (n88util
  GIT_REPOSITORY "git@github.com:Numerics88/n88util.git"
  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
  PREFIX n88util
  DEPENDS ${boost_target}
  INSTALL_DIR ${stage_location}
  CMAKE_ARGS ${project_cmake_params}
  )
set (n88util_cmake_params
  -Dn88util_DIR:PATH=${stage_location}/lib/cmake/n88util-${n88util_soversion}
  CACHE INTERNAL "cmake parameters for using n88util")

# AimIO
set (aimio_soversion 1.0)
ExternalProject_Add (aimio
  GIT_REPOSITORY "git@github.com:Numerics88/AimIO.git"
  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
  PREFIX aimio
  DEPENDS n88util ${boost_target}
  INSTALL_DIR ${stage_location}
  CMAKE_ARGS ${project_cmake_params}
  )
set (aimio_cmake_params
  -DAimIO_DIR:PATH=${stage_location}/lib/cmake/AimIO-${aimio_soversion}
  CACHE INTERNAL "cmake parameters for using aimio")

#	# Boost
#	set (boost_version 1.75.0)
#	set (extra_build_commands)
#	list (APPEND extra_build_commands link=static)
#	list (APPEND extra_build_commands cxxflags=-fPIC cflags=-fPIC)
#	set (extra_bootstrap_commands)
#	if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
#	  set (extra_bootstrap_commands --with-toolset=intel-linux)
#	endif()
#
#	ExternalProject_Add (boost
#	  URL "https://sourceforge.net/projects/boost/files/boost/${boost_version}/boost_1_75_0.tar.bz2"
#	  URL_HASH SHA256=953db31e016db7bb207f11432bef7df100516eeb746843fa0486a222e3fd49cb
#	  PREFIX boost
#	  INSTALL_DIR ${stage_location}
#	  BUILD_IN_SOURCE 1
#	  CONFIGURE_COMMAND
#	    <SOURCE_DIR>/bootstrap.sh
#	                      --prefix=<INSTALL_DIR>
#	                      --with-libraries=system
#	                      ${extra_bootstrap_commands}
#	  BUILD_COMMAND <SOURCE_DIR>/tools/build/src/engine/bjam
#	                      ${extra_build_commands}
#	  INSTALL_COMMAND <SOURCE_DIR>/tools/build/src/engine/bjam
#	                      ${extra_build_commands}
#	                      install
#	  )
#	set (boost_cmake_params
#	  -DBoost_INCLUDE_DIR:PATH=${stage_location}/include
#	  -DBoost_SYSTEM_LIBRARY_RELEASE:FILEPATH=${stage_location}/lib/libboost_system${release_suffix}.${lib_ext}
#	  -DBoost_SYSTEM_LIBRARY_DEBUG:FILEPATH=${stage_location}/lib/libboost_system${debug_suffix}.${lib_ext}
#	CACHE INTERNAL "cmake parameters for using boost")

#set(cmake_params)
#
#set(stage_location "${CMAKE_CURRENT_BINARY_DIR}/stage")
#set(SuperBuild_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
#set(SuperBuild_PROJECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Projects")
#set(SuperBuild_BINARY_DIR ${faimSuperBuild_BINARY_DIR})
#
#set(platform "unix")
#
#set(SuperBuild_MODULE_PATH)
#foreach(P IN LISTS platform)
#  list(APPEND SuperBuild_MODULE_PATH "${SuperBuild_PROJECTS_DIR}/${P}/")
#endforeach()
#list(APPEND SuperBuild_MODULE_PATH "${SuperBuild_PROJECTS_DIR}")
#list(APPEND SuperBuild_MODULE_PATH "${SuperBuild_CMAKE_DIR}")
#list(APPEND SuperBuild_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
#list(INSERT CMAKE_MODULE_PATH 0 ${SuperBuild_MODULE_PATH})
#message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
#
#set (n88util_soversion 2.0)
#
#set (N88UTIL_BUILD_SHARED_LIBS ON)
#
#set (project_cmake_params ${cmake_params})
#list (APPEND project_cmake_params
#      -DBUILD_SHARED_LIBS:BOOL=${N88UTIL_BUILD_SHARED_LIBS})
#list (APPEND project_cmake_params ${boost_cmake_params})
#
#ExternalProject_Add (n88util
#  GIT_REPOSITORY "git@github.com:Numerics88/n88util.git"
#  UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
#  PREFIX n88util
#  DEPENDS ${boost_target} ${extra_dependencies}
#  INSTALL_DIR ${stage_location}
#  CMAKE_ARGS ${project_cmake_params}
#  )
#set (n88util_target "n88util" CACHE INTERNAL "n88util target")
#
#set (n88util
#  CACHE INTERNAL "cmake parameters for using n88util")
#
#if (WIN32)
#  set (n88util_cmake_params
#    -Dn88util_DIR:PATH=${stage_location}/CMake
#    CACHE INTERNAL "cmake parameters for using n88util")
#else()
#  set (n88util_cmake_params
#    -Dn88util_DIR:PATH=${stage_location}/lib/cmake/n88util-${n88util_soversion}
#    CACHE INTERNAL "cmake parameters for using n88util")
#endif()
