set(classes
  vtkAIMReaderPlugin
  vtkboneAIMReader)

vtk_module_add_module(AIMReaders
  FORCE_STATIC # Using FORCE_STATIC build the vtk module statically into the plugin library, to avoid confusion when loading                      
  CLASSES ${classes})

paraview_add_server_manager_xmls(
  XMLS  AIMReader.xml)

add_dependencies(AIMReaders aimio)

# I cheated and replaced AimIO with hard-coded path to shared library:
# /home/buildslave/misc/code/plugin/build/stage/lib/libAimIO.so
# On CentOs I couldn't figure out how to set the AimIO library properly, so this is my hack!
# target_link_libraries (AIMReaders PUBLIC /home/buildslave/misc/code/plugin/build/stage/lib/libAimIO.so Boost::system)
target_link_libraries (AIMReaders PUBLIC AimIO Boost::system)
