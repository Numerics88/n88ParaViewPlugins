# Create plugin AIMReader
paraview_add_plugin(AIMReader
  VERSION "1.0"
  MODULES AIMReaders
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/AIMReaders/vtk.module")
