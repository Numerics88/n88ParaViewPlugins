# Build instructions for macOS

These build instructions are particular to ParaView version 5.1.2 . Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins build against it will load in the binary distribution of ParaView.

## Choice of macOS version

On macOS, as long as you use the right version of Xcode, it does not matter what version
of the operating system you use. I have built the plugins on macOS 10.11,
which then successfully load in ParaView 5.1.2 on macOS 10.10 .

## Get Xcode

You need two versions of Xcode: version 6.4 for the compiler (version 7 will not work)
and version 4.6 for the SDK.
You can download old versions of XCode from:
​https://developer.apple.com/downloads/index.action?q=xcode ,
however you need an Apple Developer account.

All versions of Xcode install by default to /Applications/Xcode.app ,
so after I install
each one, I rename it with the major version number, i.e. /Applications/Xcode4.app
and /Applications/Xcode6.app .

Set the version of Xcode to be used for compiling:

```sh
sudo xcode-select -switch /Applications/Xcode6.app
```

Verify with:

```sh
$ clang --version
Apple LLVM version 6.1.0 (clang-602.0.53) (based on LLVM 3.6.0svn)
```

## Install the binary distribution of cmake

I used the binary version 3.5.2 from http://cmake.org/cmake/resources/software.html .

## Create a case-sensitive file system for building

It seems like a case-sensitive file system is still required. The easiest is to format an additional disk volume as "Mac OS Extended (Case-sensitive, Journaled)" and to compile there. That can be done with this command:

```sh
hdiutil create -type SPARSE -fs 'Case-sensitive Journaled HFS+' -size 60g -volname CaseSensitive CaseSensitive.dmg
```

And mount it

```sh
open CaseSensitive.dmg
```

Create a working directory

```sh
cd /Volumes/CaseSensitive
mkdir -p build/ParaViewSuperbuild/v5.1.2
cd build/ParaViewSuperbuild/v5.1.2
```

## Create a working directory somewhere

You can arrange this however you want of course. Here I only give an example so subsequently
I can give concrete paths. You will have to change all the paths to your scheme.

```sh
mkdir -p ~/build/ParaViewSuperbuild/v5.1.2
cd ~/build/ParaViewSuperbuild/v5.1.2
```

## Set a minimal path

```sh
export PATH=/Applications/CMake.app/Contents/bin:/usr/bin:/bin:/usr/sbin:/sbin
```

## Get the ParaView Superbuild

```sh
git clone git://paraview.org/ParaViewSuperbuild.git
```

It is very important to get the exact version of ParaView that corresponds to these instructions:

```sh
cd ParaViewSuperbuild
git checkout v5.1.2
```

## Delete the CMake package registry

The package registry is perhaps the worst feature of CMake, since it regularly causes builds
to fail while making it nearly impossible to diagnose the cause. Before building anything with
CMake on macOS, delete any possible entries in the package registry with

```sh
rm -rf ~/.cmake
```

You should make it a habit to do this every time before running CMake.

## Configure CMake

Create a build directory:

```sh
mkdir ../build
cd ../build
ccmake ../ParaViewSuperbuild
```

Configure according to http://www.paraview.org/Wiki/ParaView/Binaries#OsX_.28macOS.29 , with some exceptions and things to note as follows::

- I recommend that you set `download_location` to some permanent directory where you store tarballs, so that it doesn't have to download everything every time. Particularly since success on the first
attempt is rare.

- Set `CMAKE_OSX_DEPLOYMENT_TARGET` to 10.7

- Set `CMAKE_OSX_SYSROOT` to `/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk`

- Turn `Paraview_FROM_GIT` to `OFF`.

- Leave `USE_NONFREE_COMPONENTS` set to `OFF`.

- `ENABLE_paraviewgettingstartedguide`, `ENABLE_paraviewtutorial`, `ENABLE_paraviewtutorialdata`, `ENABLE_paraviewusersguide` can all be `OFF`

- Make sure that `ENABLE_ospray` is `ON`, because it affects what system libraries everything links to.

- Set up python consistently:
  - `PYTHON_EXECUTABLE` should be `/usr/bin/python2.7`
  - `PYTHON_INCLUDE_DIR` should be `/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/include/python2.7`
  - `PYTHON_LIBRARY` should be `/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/lib/libpython2.7.dylib`

- If something is listed at http://www.paraview.org/Wiki/ParaView/Binaries#Linux-x64 , but the option doesn't show up, ignore it.

## Build ParaView

```sh
caffeinate make
```

Do not use the -j flag.

Go get a coffee. Or a bottle of wine. Or a long novel. Eventually, if everything works, and you
followed the instructions given above exactly, ParaView will build successfully.

## Create a working directory for building the plugins

Somewhere to build n88ParaViewPlugins and also its dependencies

```sh
mkdir -p ~/code/n88ParaViewPlugins/5.1.2
cd ~/code/n88ParaViewPlugins/5.1.2
```

## Build boost

We can't use the boost in ParaView 5.1.2, since we require at least version 1.57 of boost.

```sh
cd ~/code/n88ParaViewPlugins/5.1.2
tar xvjf boost_1_59_0.tar.bz2
cd boost_1_59_0
./bootstrap.sh --with-libraries=filesystem --with-toolset=darwin
./bjam link=static cflags="-mmacosx-version-min=10.7 -isysroot /Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk" cxxflags="-std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7 -isysroot /Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk" linkflags="-stdlib=libc++ -mmacosx-version-min=10.7 -isysroot /Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk"
export BOOST_ROOT=$HOME/code/n88ParaViewPlugins/5.1.2/boost_1_59_0
```

Tell CMake where to find boost:

```sh
export BOOST_ROOT=$HOME/code/n88ParaViewPlugins/5.1.2/boost_1_59_0
```

## Build n88util

```sh
cd ~/code/n88ParaViewPlugins/5.1.2
git clone https://github.com/Numerics88/n88util.git
cd n88util
git checkout v2.0.0
mkdir ../n88util-build
cd ../n88util-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 -DCMAKE_OSX_SYSROOT=/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" ../n88util
```

No need to actually build it, since we only need the headers (and the CMake configuration).

Tell CMake where to find n88util:

```sh
export n88util_DIR=$HOME/code/n88ParaViewPlugins/5.1.2/n88util-build
```

## Build AimIO

```sh
cd ~/code/n88ParaViewPlugins/5.1.2
git clone https://github.com/Numerics88/AimIO.git
cd AimIO
git checkout v1.0.0
mkdir ../AimIO-build
cd ../AimIO-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 -DCMAKE_OSX_SYSROOT=/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" ../AimIO
make
```

Tell CMake where to find AimIO:

```sh
export AimIO_DIR=$HOME/code/n88ParaViewPlugins/5.1.2/AimIO-build
```

## Build n88ParaViewPlugins

```sh
cd ~/code/n88ParaViewPlugins/5.1.2
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git checkout v5.1.2-1
mkdir ../n88ParaViewPlugins-build
cd ../n88ParaViewPlugins-build
export ParaView_DIR=$HOME/build/ParaViewSuperbuild/v5.1.2/build/paraview/src/paraview-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 -DCMAKE_OSX_SYSROOT=/Applications/Xcode4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" -DCMAKE_SHARED_LINKER_FLAGS="-stdlib=libc++" -DCMAKE_INSTALL_PREFIX=$HOME/code/n88ParaViewPlugins/5.1.2/install ../n88ParaViewPlugins
make
make install
```

## Fix linking for plugins

```sh
cd ../install/lib
../../n88ParaViewPlugins/fix_osx_libraries.sh /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.1.2/build libAIMReader.dylib
../../n88ParaViewPlugins/fix_osx_libraries.sh /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.1.2/build libN88ModelReader.dylib
../../n88ParaViewPlugins/fix_osx_libraries.sh /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.1.2/build libImageGaussianSmooth.dylib
```

If everything worked correctly, the plugins will be in $HOME/code/n88ParaViewPlugins/5.1.2/install/lib .
They can be copied to another system and loaded in ParaView 5.1.2 .
