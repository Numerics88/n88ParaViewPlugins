set(classes
  vtkAIMReaderPlugin
  vtkboneAIMReader)

vtk_module_add_module(AIMReaders
  FORCE_STATIC # Using FORCE_STATIC build the vtk module statically into the plugin library, to avoid confusion when loading                      
  CLASSES ${classes})

paraview_add_server_manager_xmls(
  XMLS  AIMReader.xml)

add_dependencies(AIMReaders aimio)

target_link_libraries (AIMReaders LINK_PUBLIC AimIO Boost::system)