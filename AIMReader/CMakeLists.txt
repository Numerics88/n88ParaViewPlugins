# Requires n88util
find_package (n88util 2.0 REQUIRED)
include_directories (${N88UTIL_INCLUDE_DIRS})

# Requires AimIO
find_package (AimIO 1.0 REQUIRED)
include_directories (${AIMIO_INCLUDE_DIRS})

add_definitions ("-DN88_BUILD_AS_PARAVIEW_PLUGIN")

add_paraview_plugin(
    AIMReader
    "1.0"
    SERVER_MANAGER_XML
      AIMReader.xml
    SERVER_MANAGER_SOURCES
      vtkAIMReaderPlugin.cxx
    SOURCES
      vtkboneAIMReader.cxx)

target_link_libraries (AIMReader LINK_PUBLIC AimIO)

install (TARGETS AIMReader
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib)
