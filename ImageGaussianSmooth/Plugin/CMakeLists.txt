# Create plugin ImageGaussianSmooth
paraview_add_plugin(ImageGaussianSmooth
  VERSION "1.0"
  REQUIRED_ON_SERVER
  SERVER_MANAGER_XML ImageGaussianSmooth.xml)
