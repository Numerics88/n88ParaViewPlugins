# Build instructions for Windows

These build instructions are particular to ParaView version 5.1.2 . Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins build against it will load in the binary distribution of ParaView.

## Choice of operating system

As long as you use the specified version of Visual Studio, it shouldn't matter which
version of Windows you build on. I used Windows 10.

## Install Visual Studio

You need Visual Studio 2013.

## Install python

I use anaconda python.

## Install 7-zip

Does not necessary need to be on the path; you can just specify its location in CMake.

## Install git

You need some version of git. I used Github Desktop. https://desktop.github.com/

## Install jam

Get from http://sourceforge.net/projects/freetype/files/ftjam/2.5.2/ .

## Install ninja

Start a VS2013 x64 Native Tools Command Prompt

```
git clone git://github.com/martine/ninja.git
cd ninja
git checkout v1.7.1
python configure.py --bootstrap
```

The only thing needed is the binary ninja.exe . I make a directory c:\Program Files\ninja and put it in there. Then I put that directory on the Path.

## Install CMake

I used the binary version 3.5.2 from http://cmake.org/cmake/resources/software.html .

## Get the ParaViewSuperbuild source code

```sh
cd %HOMEDRIVE%%HOMEPATH%\build
git clone http://paraview.org/ParaViewSuperbuild.git
cd ParaViewSuperbuild
git checkout v5.1.2
```

## Create a working directory somewhere

To avoid problems with file name length, you have to create a build directory that is right
at the root level. For example

```sh
mkdir c:\pv-5.1
```

Also, apparently the build directory needs to be on the same drive as the ParaView source code.

## Set a minimal path

A particular problem is that git installations have a program called `link`; so does
Visual Studio. You may have to customize the following for your system.

```
set PATH=C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\CommonExtensions\Microsoft\TestWindow;C:\Program Files (x86)\MSBuild\12.0\bin\amd64;C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\BIN\amd64;C:\WINDOWS\Microsoft.NET\Framework64\v4.0.30319;C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\VCPackages;C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE;C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools;C:\Program Files (x86)\HTML Help Workshop;C:\Program Files (x86)\Microsoft Visual Studio 12.0\Team Tools\Performance Tools\x64;C:\Program Files (x86)\Microsoft Visual Studio 12.0\Team Tools\Performance Tools;C:\Program Files (x86)\Windows Kits\8.1\bin\x64;C:\Program Files (x86)\Windows Kits\8.1\bin\x86;C:\Program Files (x86)\Microsoft SDKs\Windows\v8.1A\bin\NETFX 4.5.1 Tools\x64\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\ninja
```

## Delete the CMake package registry

The package registry is perhaps the worst feature of CMake, since it regularly causes builds
to fail while making it nearly impossible to diagnose the cause. Before building anything with
CMake on Windows,
open the Registry Editor (regedit) and delete HKEY_CURRENT_USER\Software\Kitware\CMake\Packages
and all its sub-keys.

You should make it a habit to do this every time before running CMake.

## Configure CMake

Make sure that you are (still) running a VS2013 x64 Native Tools Command Prompt, then

```sh
cd c:\pv-5.1
cmake-gui %HOMEDRIVE%%HOMEPATH%\build\ParaViewSuperbuild
```

And choose Ninja as the generator. The compiler version should be 18.

Configure according to http://www.paraview.org/Wiki/ParaView/Binaries#Windows-x64 , with some exceptions and things to note as follows::

- I recommend that you set `download_location` to some permanent directory where you store tarballs, so that it doesn't have to download everything every time. Particularly since success on the first
attempt is rare.

- Turn `Paraview_FROM_GIT` to `OFF`.

- Leave `USE_NONFREE_COMPONENTS` set to `OFF`.

- `ENABLE_paraviewgettingstartedguide`, `ENABLE_paraviewtutorial`, `ENABLE_paraviewtutorialdata`, `ENABLE_paraviewusersguide` can all be `OFF`

- Set `ENABLE_mpi` to `OFF`

- Set `ENABLE_silo` to `OFF`

- Set `ENABLE_vortexfinder2` to `OFF`

- The page http://www.paraview.org/Wiki/ParaView/Binaries#Windows-x64 suggests using a pre-build
  Qt, but it is not clear where they get a pre-built Qt 4.8.4 for VS 2013, so leave
  `USE_SYSTEM_qt4` to `OFF`.

- If something is listed at http://www.paraview.org/Wiki/ParaView/Binaries#Windows-x64 , but the option doesn't show up, ignore it.

## Build ParaView

```sh
ninja -j 1
```

Go get a coffee. Or a bottle of wine. Or a long novel. Eventually, if everything works, and you
followed the instructions given above exactly, ParaView will build successfully.

## Create a working directory for building the plugins

Somewhere to build n88ParaViewPlugins and also its dependencies

```sh
cd %HOMEDRIVE%%HOMEPATH%
mkdir code\n88ParaViewPlugins\5.1.2
cd code\n88ParaViewPlugins\5.1.2
```

## Build boost

We can't use the boost in ParaView 5.1.2, since we require at least version 1.57 of boost.

```sh
"c:\Program Files\7-Zip\7z.exe" x boost_1_59_0.7z
cd boost_1_59_0
bootstrap.bat
b2 --layout=tagged --with-filesystem --with-date_time --with-thread --with-timer --with-program_options toolset=msvc-12.0 link=static threading=multi address-model=64
```

Tell CMake where to find boost:

```sh
set BOOST_ROOT=%HOMEDRIVE%%HOMEPATH%\code\n88ParaViewPlugins\5.1.2\boost_1_59_0
```

## Build n88util

```sh
cd ..
git clone https://github.com/Numerics88/n88util.git
cd n88util
git checkout v2.0.0
mkdir ..\n88util-build
cd ..\n88util-build
"c:\Program Files (x86)\CMake\bin\cmake.exe" -G Ninja -DCMAKE_BUILD_TYPE=Release ..\n88util
```

No need to actually build it, since we only need the headers (and the CMake configuration).

Tell CMake where to find n88util:

```sh
set n88util_DIR=%HOMEDRIVE%%HOMEPATH%\code\n88ParaViewPlugins\5.1.2\n88util-build
```

## Build AimIO

```sh
cd ..
git clone https://github.com/Numerics88/AimIO.git
cd AimIO
git checkout v1.0.0
mkdir ../AimIO-build
cd ../AimIO-build
"c:\Program Files (x86)\CMake\bin\cmake.exe" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBoost_SYSTEM_LIBRARY_RELEASE=%BOOST_ROOT%\stage\lib\libboost_system-vc120-mt-1_59.lib -DBoost_FILESYSTEM_LIBRARY_RELEASE=%BOOST_ROOT%\stage\lib\libboost_filesystem-vc120-mt-1_59.lib ..\AimIO
ninja -j 1
```

Tell CMake where to find AimIO:

```sh
set AimIO_DIR=%HOMEDRIVE%%HOMEPATH%\code\n88ParaViewPlugins\5.1.2\AimIO-build
```

## Build n88ParaViewPlugins

```sh
cd ..
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git checkout v1.0.0
mkdir ../n88ParaViewPlugins-build
cd ../n88ParaViewPlugins-build
set ParaView_DIR=C:\pv-5.1\paraview\src\paraview-build
"c:\Program Files (x86)\CMake\bin\cmake.exe" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%HOMEDRIVE%%HOMEPATH%\code\n88ParaViewPlugins\5.1.2\install ..\n88ParaViewPlugins
ninja -j 1
ninja install
```

If everything worked correctly, the plugins will be in %HOMEDRIVE%%HOMEPATH%\code\n88ParaViewPlugins\5.1.2\install\bin .
They can be copied to another system and loaded in ParaView 5.1.2 .
