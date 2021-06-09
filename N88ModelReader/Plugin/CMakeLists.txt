# Create plugin N88ModelReader
paraview_add_plugin(N88ModelReader
  VERSION "1.0"
  MODULES N88ModelReaders
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/N88ModelReaders/vtk.module")
