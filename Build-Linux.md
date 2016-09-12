# Build instructions for Linux

These build instructions are particular to ParaView version 5.1.2 . Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins build against it will load in the binary distribution of ParaView.

## Build on Debian 7

I have not successfully built ParaView on Debian 6, as claimed on
http://www.paraview.org/Wiki/ParaView/Binaries#Linux-x64 . However, Debian 7 works. And judging by
the system libraries that the binary distribution of ParaView links against, I believe that Kitware
does also in fact use Debian 7.

It might also be possible to use a newer distro of Linux, but then the plugins will be linked
against more recent versions of the standard system libraries, and can't be used with older
distros.

Since Debian 7 is probably not your day-to-day installation of Linux, he easiest thing is to run
Debian 7 inside a virtual machine using Virtual Box. See https://www.virtualbox.org/ . On Ubuntu
Linux for example, you can install Virtual Box with

```sh
sudo apt-get install virtualbox
```

The standard system gcc (version 4.7.2) for Debian 7 is fine: you don't need to use gcc 4.8 as long as you follow the instructions below.

## Install the binary distribution of cmake

I used the Linux binary version 3.5.2 from http://cmake.org/cmake/resources/software.html .

## Required packages for ParaView Superbuild

The following command should install all the required packages on Debian 7:

```sh
sudo apt-get install build-essential libstdc++6 libc6-dev-i386 libglu1-mesa-dev freeglut3-dev subversion libxmu-dev libxi-dev gfortran libxt-dev libxrender-dev doxygen
```

## Create a working directory somewhere

You can arrange this however you want of course. Here I only give an example so subsequently
I can give concrete paths. You will have to change all the paths to your scheme.

```sh
mkdir -p ~/build/ParaViewSuperbuild/v5.1.2
cd ~/build/ParaViewSuperbuild/v5.1.2
```

## Set a minimal path and a library search path

```sh
export PATH=/opt/cmake-3.5.2-Linux-x86_64/bin:/usr/local/bin:/usr/local/bin:/usr/bin:/bin
export LD_LIBRARY_PATH=$HOME/build/ParaViewSuperbuild/v5.1.2/build/install/lib:$HOME/build/ParaViewSuperbuild/v5.1.2/build/qt/src/qt-build/lib
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


## Configure CMake

Create a build directory:

```sh
mkdir ../build
cd ../build
ccmake ../ParaViewSuperbuild
```

Configure according to http://www.paraview.org/Wiki/ParaView/Binaries#Linux-x64 , with some exceptions and things to note as follows::

- I recommend that you set `download_location` to some permanent directory where you store tarballs, so that it doesn't have to download everything every time. Particularly since success on the first
attempt is rare.

- Turn `Paraview_FROM_GIT` to `OFF`.

- Leave `USE_NONFREE_COMPONENTS` set to `OFF`.

- `ENABLE_paraviewgettingstartedguide`, `ENABLE_paraviewtutorial`, `ENABLE_paraviewtutorialdata`, `ENABLE_paraviewusersguide` can all be `OFF`

- Set `ENABLE_ospray` to `OFF`, because it wants at least gcc 4.8

- If something is listed at http://www.paraview.org/Wiki/ParaView/Binaries#Linux-x64 , but the option doesn't show up, ignore it.


## Build ParaView

```sh
make
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
./bootstrap.sh --with-libraries=filesystem
./bjam link=static
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
cmake -DCMAKE_BUILD_TYPE=Release ../n88util
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
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC ../AimIO
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
git checkout v1.0.0
mkdir ../n88ParaViewPlugins-build
cd ../n88ParaViewPlugins-build
export ParaView_DIR=$HOME/build/ParaViewSuperbuild/v5.1.2/build/paraview/src/paraview-build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/code/n88ParaViewPlugins/5.1.2/install ../n88ParaViewPlugins
make
make install
```

If everything worked correctly, the plugins will be in $HOME/code/n88ParaViewPlugins/5.1.2/install .
They can be copied to another system and loaded in ParaView 5.1.2 .
