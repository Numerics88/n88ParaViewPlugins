add_paraview_plugin(
    ImageGaussianSmooth
    "1.0"
    SERVER_MANAGER_XML ImageGaussianSmooth.xml)

install (TARGETS ImageGaussianSmooth
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib)

