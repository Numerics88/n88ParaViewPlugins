add_paraview_plugin(
    N88ModelReader
    "1.0"
    SERVER_MANAGER_XML N88ModelReader.xml
    SERVER_MANAGER_SOURCES vtkN88ModelReaderPlugin.cxx)

target_link_libraries (N88ModelReader LINK_PUBLIC
    ${vtknetcdf_LIBRARIES}
    ${vtkhdf5_LIBRARIES})

install (TARGETS N88ModelReader
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib)

