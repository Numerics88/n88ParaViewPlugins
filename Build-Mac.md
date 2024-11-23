# Build instructions for macOS

These build instructions are particular to ParaView version 5.13.1.
Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins built against it will load in the binary distribution of ParaView.

Check your mental health before you try and build these plugins...

## Learn more about plugins

The C++ documentation contains dedicated pages:
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginHowto.html
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginMigration.html
- https://www.paraview.org/paraview-docs/nightly/cxx/index.html

The source code contains many plugin examples.
- https://gitlab.kitware.com/paraview/paraview/-/tree/master/Examples/Plugins

Note that any documentation on the ParaView wiki is out of date.

## Choice of macOS version

On macOS, as long as you have Apple's developer tools, it does not matter what version
of the operating system you use. I have built the plugins on macOS 10.11,
which then successfully load in ParaView 5.1.2 on macOS 10.10 .

## Get Xcode or the command line developer tools

Apple has made it very easy to get their compiler and SDK.  All Macs come with a little utility
for installing a compiler that you can use in place of the full IDE that comes with Xcode.
The first thing to do is check whether the compiler is already installed:

    xcode-select --print-path

If you already have a compiler, it will print something like this:

    /Library/Developer/CommandLineTools

If it doesn't print a path to either "CommandLineTools" or to "Xcode.app", then you will have
to do one of two things: either install Xcode from the App store or from Apple's developer site,
which might take around 20GB of disk space, or just install the compiler and SDK, which will take
around 2GB of disk space.  To do the latter,

    xcode-select --install

This command will bring up a dialog box where you can agree to Apple's license terms and
install the lightweight version of Apple's developer tools.  This will provide everything
you need to build ParaView.

If you start with the lightweight option, but later decide to install Xcode, you can set
the command-line "clang" to be the one in Xcode by executing the following line:

    sudo xcode-select --switch /Applications/Xcode.app

You can verify which version of "clang" you are using:

    clang --version

    Apple clang version 14.0.0 (clang-1400.0.29.202)
    Target: x86_64-apple-darwin21.6.0
    Thread model: posix
    InstalledDir: /Library/Developer/CommandLineTools/usr/bin

Also, once you know where "clang" is installed, you can check which SDKs are available:

    ls /Library/Developer/CommandLineTools/SDKs

    MacOSX13.1.sdk
    MacOSX14.2.sdk

Currently, paraview release packages are built with MacOSX13.1.sdk (more details on this below).
Sometimes, even when "superbuild" is told to use a specific SDK, projects in the superbuild will
find other SDKs.  In one instance, the superbuild built Python with one SDK but attepted to build
some of Python's modules (in this case, `_scproxy.c`) with a different SDK.  If there are errors
during the superbuild build of Python, you can try temporarily moving all but your desired SDK out
of the SDKs directory, and then retrying the build.

## Install the binary distribution of cmake

You can install binary packages of cmake from https://cmake.org/download which will place cmake
in your /Applications folder.  For superbuild, cmake 3.26 or later is recommended.

Alternatively, and my preference, I use Anaconda (https://docs.conda.io/en/latest/miniconda.html) to set up a conda environment. 

```sh
$ conda create -n paraview -c conda-forge cmake=3.26 python=3.10
$ conda activate paraview
```

## Choose working directory

You can arrange this however you want of course. Here I only give an example as a starting point,
most of the instructions use relative paths.

```sh
cd ~
mkdir -p Development/Numerics88
cd Development/Numerics88
```

## Set a minimal path

It's important to ensure that when cmake is configuring ParaView, it doesn't find any libraries
or headers installed from homebrew or elsewhere.  Set a minimal path that includes only the
directories that are needed.

```sh
export PATH=/Applications/CMake.app/Contents/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
```
Or if you used a `conda` environment:

```sh
export PATH=/Users/skboyd/.conda/envs/paraview/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
```

## Get the ParaView Superbuild

Rather than cloning ParaView itself, you must clone the ParaView superbuild repository:

```sh
git clone --recursive https://gitlab.kitware.com/paraview/paraview-superbuild.git
```

Since our goal is to build plugins for the most recent stable release of paraview, we use the
'release' branch of paraview-superbuild.

```sh
cd paraview-superbuild
git checkout release
git submodule update
```

The paraview-superbuild directory has a subdirectory called "superbuild", which is actually a
separate git repository.  In order to get netcdf to build for Paraview 5.13.1, you might need
to bring in a netcdf-specific patch as follows (the starting point here is the paraview-superbuild
directory):

```sh
cd superbuild
git merge --ff-only 3aa9b824
cd ..
```
This patch fixes a build error in the code where netcdf loads hdf5 files (file: hdf5open.c).


## Configure CMake

Create build and downloads directories (do this while you are in the paraview-superbuild directory).
The downloads directory is where the superbuild will cache any tarballs it downloads.

```sh
mkdir ../paraview-superbuild-build
mkdir ../paraview-superbuild-downloads
cd ../paraview-superbuild-build
ccmake ../paraview-superbuild
```

The following settings are recommended:
- Set `BUILD_SHARED_LIBS` to `ON` (default).
- Set `superbuild_download_location` to the downloads directory you created
- Set `CMAKE_OSX_ARCHITECTURES` to `x86_64` for Intel CPU, or `arm64` for Apple CPU
- Set `CMAKE_OSX_DEPLOYMENT_TARGET` to 10.15 for Intel CPU, or 11.0 for Apple CPU
- Set `CMAKE_OSX_SYSROOT` to `/Library/Developer/CommandLineTools/SDKs/MacOSX13.1.sdk`
- Set `ENABLE_ospray` to `ON`. This affects which system libraries everything links to (e.g. netcdf, hdf5, etc).
- Set `ENABLE_hdf5` to `ON`.
- Set `ENABLE_netcdf` to `ON`.
- Set `ENABLE_python3` to `ON`.

If you have the full Xcode instead of the developer command line tools, use this as the SDK:
- `/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk`

Some further settings that will pop up in subsequent iterations of the configuration process are: 
- Set `BUILD_TESTING` to `OFF`.
- Set `USE_SYSTEM_hdf5` to `OFF` (default).
- Set `USE_SYSTEM_netcdf` to `OFF` (default).
- Set `USE_SYSTEM_python3` to `OFF` (default).

If and *only* if you want to install the ParaView GUI from the superbuild, enable Qt.  This is not recommended,
since it's much easier to download the ParaView binary package instead.
- Set `ENABLE_qt5` to `ON` (this will add build time and requires Qt!)
- In the shell, `export CMAKE_PREFIX_PATH=/path/to/Qt5.12.12/5.12.12/clang_64`

## Build ParaView

Use `caffeinate` to keep the computer awake until `make` is done:

```sh
caffeinate make
```

If you use the `-j` flag, for example `-j8`, then use it with caution.  It works with recent versions
of the superbuild, but it has failed in the past and might fail again in the future.

Go get a coffee. Or a bottle of wine. Or a long novel. Eventually, if everything works, and you 
followed the instructions given above exactly, ParaView will build successfully.

## Build n88ParaViewPlugins

Choose a location to build n88ParaViewPlugins.  This build is similar to a superbuild,
in that it downloads dependencies like Boost and AimIO automatically.  Ideally, you want
to use the version of n88ParaViewPlugins that matches ParaView itself, but you can try
the master branch if no exact match is available.

```sh
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git tag # available versions
git checkout v5.13.1
mkdir -p ../n88ParaViewPlugins-build/v5.13.1
cd ../n88ParaViewPlugins-build/v5.13.1
```

Next, some environment variable must be set, so that cmake can find what it needs from ParaView.
The first environment variable will be the location of the ParaView superbuild.  What I have
written below is just an example.
```sh
export PVSB=/Users/username/Development/Numerics88/paraview-superbuild-build
```
Then several other variables:

```sh
export ParaView_DIR=${PVSB}/superbuild/paraview/build
export rkcommon_DIR=${PVSB}/install/lib/cmake/rkcommon-1.7.0/
export openvkl_DIR=${PVSB}/install/lib/cmake/openvkl-1.0.1/
export netCDF_DIR=${PVSB}/install/lib/cmake/netCDF
export nlohmann_json_DIR=${PVSB}/superbuild/nlohmannjson/build/
export ZLIB_ROOT=${PVSB}/install
```

Finally, you are ready to do the build.  Make sure the `ARCHITECTURES` is set to `x86_64` or
`arm64`, according to whether you have an Intel CPU or an Apple CPU, and make sure that
the `DEPLOYMENT_TARGET` matches the macOS version stamped on the binary ParaView package:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD:STRING=14 \
      -DCMAKE_OSX_ARCHITECTURES:STRING=arm64 \
      -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=11.0 \
      -DCMAKE_OSX_SYSROOT:PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX13.1.sdk \
      -DCMAKE_INSTALL_PREFIX=`pwd`/install \
      -DPNG_PNG_INCLUDE_DIR:PATH=${PVSB}/install/include/libpng16 \
      -DPNG_LIBRARY_RELEASE:FILEPATH=${PVSB}/install/lib/libpng16.16.dylib \
      ../n88ParaViewPlugins
make
make install
```

This build will go much faster than the ParaView superbuild.  But you're not done yet, because
if you try using these plugins in ParaView as-is, then you will get a nasty error like this:

    Algorithm vtkAIMReaderPlugin (0x6000026a1fb0) did not create output for port 0 when
    asked by REQUEST_DATA_OBJECT and does not specify any DATA_TYPE_NAME.

Please read the next section for the fix.

## Fix linking for plugins

The dynamic linked libraries need some repair, so we use a script to fix the .so files.
As in the previous section, `PVSB` is the ParaView superbuild director.

```sh
pushd install/lib/paraview-5.13/plugins
cd ImageGaussianSmooth
../../../../../../n88ParaViewPlugins/fix_osx_libraries.sh ${PVSB} ImageGaussianSmooth.so
cd ../AIMReader
../../../../../../n88ParaViewPlugins/fix_osx_libraries.sh ${PVSB} AIMReader.so
cd ../N88ModelReader
../../../../../../n88ParaViewPlugins/fix_osx_libraries.sh ${PVSB} N88ModelReader.so
popd
```

If everything worked correctly, the plugins will be in `install/lib/paraview-5.13/plugins`
in your current directory (that is, the directory where you ran cmake and make).

## Create an archive for distribution

Try something like this to create a plugin package.

```sh
mkdir -p ~/Downloads/Numerics88/Plugins/ParaView-5.13
cp install/lib/paraview-5.13/plugins/*/*.so ~/Downloads/Numerics88/Plugins/ParaView-5.13
cd ~/Downloads
tar -cjvSf n88ParaViewPlugins-5.13.1-Mac-arm64.tar.bz2 ./Numerics88
```

Then you can extract the archive like this:

```sh
tar -xjvf n88ParaViewPlugins-5.13.1-Mac-arm64.tar.bz2
```
