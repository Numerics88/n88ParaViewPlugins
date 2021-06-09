set(classes
	vtkN88ModelReaderPlugin)

vtk_module_add_module(N88ModelReaders
  FORCE_STATIC # Using FORCE_STATIC build the vtk module statically into the plugin library, to avoid confusion when loading                      
  CLASSES ${classes})

paraview_add_server_manager_xmls(
  XMLS  N88ModelReader.xml)

target_link_libraries (N88ModelReaders LINK_PUBLIC
    VTK::netcdf
    VTK::hdf5)
