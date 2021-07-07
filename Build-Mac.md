# Build instructions for macOS

These build instructions are particular to ParaView version 5.9.1 . 
Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins build against it will load in the binary distribution of ParaView.

Check your mental health before you try and build these plugins...

## Learn more about plugins
The C++ documentation contains dedicated pages:
https://kitware.github.io/paraview-docs/latest/cxx/PluginHowto.html
https://kitware.github.io/paraview-docs/latest/cxx/PluginMigration.html
https://kitware.github.io/paraview-docs/latest/cxx/index.html

The source code contains many plugin examples.
https://gitlab.kitware.com/paraview/paraview/-/tree/master/Examples/Plugins

The wiki used to be the official resources but it is being phased out.
## Choice of macOS version

On macOS, as long as you use the right version of Xcode, it does not matter what version
of the operating system you use. I have built the plugins on macOS 10.11,
which then successfully load in ParaView 5.1.2 on macOS 10.10 .

## Get Xcode

You used to need two versions of Xcode: version 6.4 for the compiler (version 7 will not work)
and version 4.6 for the SDK. However, this issue seems to be resolved now (9-JUN-2021). Just in
case you can download old versions of XCode from:
​https://developer.apple.com/downloads/index.action?q=xcode ,
however you need an Apple Developer account.

All versions of Xcode install by default to /Applications/Xcode.app , so after I install each one, 
I rename it with the major version number, i.e. /Applications/Xcode4.app and /Applications/Xcode6.app .

Set the version of Xcode to be used for compiling:

```sh
sudo xcode-select -switch /Applications/Xcode6.app
```

Verify with:

```sh
$ clang --version
Apple LLVM version 6.1.0 (clang-602.0.53) (based on LLVM 3.6.0svn)
```
But for 2021, I'm not doing any of the above and instead using the base installation of Xcode on my system:

```s
$ clang --version
Apple clang version 12.0.0 (clang-1200.0.32.27)
Target: x86_64-apple-darwin19.6.0
```
## Create a case-sensitive file system for building

It seems like a case-sensitive file system is still required. The easiest is to format an additional disk 
volume as "Mac OS Extended (Case-sensitive, Journaled)" and to compile there. That can be done with this command:

```sh
cd ~/Desktop/
hdiutil create -type SPARSE -fs 'Case-sensitive Journaled HFS+' -size 60g -volname CaseSensitive CaseSensitive.dmg
```

And mount it

```sh
open CaseSensitive.dmg.sparseimage
```

Create a working directory

```sh
cd /Volumes/CaseSensitive
mkdir -p build/ParaViewSuperbuild/v5.9.1
cd build/ParaViewSuperbuild/v5.9.1
```

## Install the binary distribution of cmake

You can install the binary version 3.18.4 from http://cmake.org/cmake/resources/software.html .

Alternatively, and my preference, I use Anaconda (https://docs.conda.io/en/latest/miniconda.html) to set up a conda environment. 

```sh
$ conda create -n paraview -c conda-forge cmake=3.18 python=3.7
$ conda activate paraview
```

## Start with your working directory

You can arrange this however you want of course. Here I only give an example so subsequently
I can give concrete paths. You will have to change all the paths to your scheme.

```sh
cd /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1
```

## Set a minimal path

```sh
export PATH=/Applications/CMake.app/Contents/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
```
Or if you used a `conda` environment:

```sh
export PATH=/Users/skboyd/.conda/envs/paraview/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
```

## Get the ParaView Superbuild

```sh
git clone --recursive https://gitlab.kitware.com/paraview/paraview-superbuild.git
```

It is very important to get the exact version of ParaView that corresponds to these instructions:

```sh
cd paraview-superbuild
git checkout v5.9.1
git submodule update
```

## Configure CMake

Create a build directory:

```sh
mkdir ../build
cd ../build
ccmake ../paraview-superbuild
```
The following settings are recommended:
– Set `BUILD_SHARED_LIBS` to `ON` (default).
- Set `superbuild_download_location` to some permanent directory where you store tarballs.
- Set `CMAKE_OSX_DEPLOYMENT_TARGET` to 10.13
- Set `CMAKE_OSX_SYSROOT` to `/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk`
- Set `USE_NONFREE_COMPONENTS` to `OFF` (default).
- Set `OFF` for the following (defaults):
	- `ENABLE_paraviewgettingstartedguide`
	- `ENABLE_paraviewtutorialdata`
- Set `ENABLE_ospray` to `ON`. This affects which system libraries everything links to (e.g. netcdf, hdf5, etc).
- Set `ENABLE_hdf5` to `ON`.
- Set `ENABLE_netcdf` to `ON`.

Some further settings that will pop up in subsequent iterations of the configuration process are: 
- Set `BUILD_TESTING` to `OFF`.
- Set `USE_SYSTEM_hdf5` to `OFF` (default).
- Set `USE_SYSTEM_netcdf` to `OFF` (default).
- Set `USE_SYSTEM_python3` to `OFF` (default).

## Build ParaView

```sh
caffeinate make
```

Do not use the `-j` flag.

Go get a coffee. Or a bottle of wine. Or a long novel. Eventually, if everything works, and you 
followed the instructions given above exactly, ParaView will build successfully.

## Create a working directory for building the plugins

Somewhere to build n88ParaViewPlugins and also its dependencies

```sh
cd /Volumes/CaseSensitive/build
mkdir -p n88ParaViewPlugins/v5.9.1
cd ~/n88ParaViewPlugins/v5.9.1
```

## Build boost

We probably can use the boost in ParaView 5.9.1, but previous ParaView versions did not have an appropriate boost version (e.g. v1.57), so to be safe we grab our own version from here:

https://sourceforge.net/projects/boost/files/boost/1.75.0/boost_1_75_0.tar.bz2

```sh
cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1
cp ~/Downloads/boost_1_75_0.tar.bz2 .
tar xvjf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh --with-libraries=filesystem,system --with-toolset=darwin
./b2 link=static cflags="-mmacosx-version-min=10.13 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk" cxxflags="-std=c++11 -stdlib=libc++ -mmacosx-version-min=10.13 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk" linkflags="-stdlib=libc++ -mmacosx-version-min=10.13 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk"
```

Tell CMake where to find boost:

```sh
export BOOST_ROOT=/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/boost_1_75_0
```

## Build n88util

```sh
cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/n88util.git
cd n88util
git checkout v2.0.0
mkdir ../n88util-build
cd ../n88util-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" ../n88util
make
```

Actually, there is no need to actually build it, since we only need the headers (and the CMake configuration). I make it out of habit.

Tell CMake where to find n88util:

```sh
export n88util_DIR=/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88util-build
```

## Build AimIO

```sh
cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/AimIO.git
cd AimIO
git checkout v1.0.0
mkdir ../AimIO-build
cd ../AimIO-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" ../AimIO
make
```

Tell CMake where to find AimIO:

```sh
export AimIO_DIR=/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/AimIO-build
```

## Build n88ParaViewPlugins

Make sure that N88_BUILD_APPLE is set OFF in 'n88ParaViewPlugins/AIMReader/CMakeLists.txt'

```sh
cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git checkout v5.9.1
mkdir ../n88ParaViewPlugins-build
cd ../n88ParaViewPlugins-build
export ParaView_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/superbuild/paraview/build
export rkcommon_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/rkcommon-1.5.1/
export openvkl_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/openvkl-0.11.0/
export netCDF_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/netCDF
export nlohmann_json_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/superbuild/nlohmannjson/build/
export openvkl_DIR=/Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/openvkl-0.11.0/
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.0.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" -DCMAKE_SHARED_LINKER_FLAGS="-stdlib=libc++" -DCMAKE_INSTALL_PREFIX=/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install ../n88ParaViewPlugins
make
make install
```

## Fix linking for plugins

The dynamic linked libraries need some repair, so we use a script to fix the .so files:

```sh
cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install/lib/paraview-5.9/plugins/ImageGaussianSmooth
/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/fix_osx_libraries.sh \
  /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build ImageGaussianSmooth.so

cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install/lib/paraview-5.9/plugins/AIMReader
/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/fix_osx_libraries.sh \
  /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build AIMReader.so

cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install/lib/paraview-5.9/plugins/N88ModelReader
/Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/fix_osx_libraries.sh \
  /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build N88ModelReader.so
```

If everything worked correctly, the plugins will be in /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install/lib/paraview-5.9/plugins/ .
They can be copied to another system and loaded in ParaView 5.9.1 . 

## Create an archive for distribution

Try something like this to create the archive after you move the .so files to the ~/Downloads folder from 

```sh
cp /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/install/lib/paraview-5.9/plugins/*/*.so ~/Downloads
cd ~/Downloads
mkdir -p Numerics88/Plugins/ParaView-5.9
cp *.so Numerics88/Plugins/ParaView-5.9
tar -cvjSf n88ParaViewPlugins-5.9.1-Mac.tar.bz2 ./Numerics88
```

Then you can extract the archive like this:

```sh
tar xvfj n88ParaViewPlugins-5.9.1-Mac.tar.bz2
```

