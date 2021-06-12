# Build instructions for Linux

These build instructions are particular to ParaView version 5.9.1 . Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins build against it will load in the binary distribution of ParaView.

I built on TheGNU (CentOs Linux v7).

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

These instructions above are from building 5.1.2 (i.e. old), and for 5.9.1 I've built using CentOs v7.

## Create a virtual machine

I created a virtual machine on AWS at aws.amazon.com. You could try and install Paraview on it. 
This is only useful for testing whether we can load the final plugins. To build
the plugins we need the source code as well.

```sh
wget https://www.paraview.org/files/v5.9/ParaView-5.9.1-MPI-Linux-Python3.8-64bit.tar.gz
```
You may need to update apt:
```sh
sudo apt-get update
```
This command gets everything we need:
```sh
sudo apt-get install build-essential libstdc++6 libc6-dev-i386 libglu1-mesa-dev freeglut3-dev subversion libxmu-dev libxi-dev gfortran libxt-dev libxrender-dev doxygen
```
If using a free virtual machine it may have limited RAM. A fix is
to pay for more RAM, or you can create swap space:

```sh
sudo fallocate -l 1G /swapfile 
sudo chmod 600 /swapfile 
sudo mkswap /swapfile 
sudo swapon /swapfile 
sudo cp /etc/fstab /etc/fstab.bak 
echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab
```

## Install cmake

You can install the binary version 3.18.4 from http://cmake.org/cmake/resources/software.html .

Alternatively, and my preference, I use Anaconda (https://docs.conda.io/en/latest/miniconda.html) to set up a conda environment. 

```sh
wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash ~/Miniconda3-latest-Linux-x86_64.sh
source .bashrc
conda update conda
rm ~/Miniconda3-latest-Linux-x86_64.sh
```

Create your environment:
```sh
conda create -n paraview -c conda-forge cmake=3.18 python=3.8
conda activate paraview
```

## Start with your working directory

You can arrange this however you want of course. Here I only give an example so subsequently
I can give concrete paths. You will have to change all the paths to your scheme.

```sh
mkdir -p ~/build/ParaViewSuperbuild/v5.9.1
cd ~/build/ParaViewSuperbuild/v5.9.1
```

## Set a minimal path and a library search path

```sh
export PATH=/home/fem/anaconda2/envs/faim-devel/bin:/home/fem/anaconda2/bin:/home/fem/anaconda2/condabin:/usr/bin:/usr/sbin
export LD_LIBRARY_PATH=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/lib:/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/qt/src/qt-build/lib
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
- Set `USE_NONFREE_COMPONENTS` to `OFF` (default).
- Set `OFF` for the following (defaults):
	- `ENABLE_paraviewgettingstartedguide`
	- `ENABLE_paraviewtutorialdata`
- Set `ENABLE_python3` to `ON`.
- Set `ENABLE_ospray` to `OFF`. Not sure why this won't build; maybe GCC version related.
- Set `ENABLE_hdf5` to `ON`.
- Set `ENABLE_netcdf` to `ON`.

Some further settings that will pop up in subsequent iterations of the configuration process are: 
- Set `BUILD_TESTING` to `OFF`.
- Set `USE_SYSTEM_hdf5` to `OFF` (default).
- Set `USE_SYSTEM_netcdf` to `OFF` (default).
- Set `USE_SYSTEM_python3` to `OFF` (default).


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
cd /home/fem/faim-devel/build
mkdir -p n88ParaViewPlugins/v5.9.1
cd n88ParaViewPlugins/v5.9.1
```

## Build boost

We probably can use the boost in ParaView 5.9.1, but previous ParaView versions did not have an appropriate boost version (e.g. v1.57), so to be safe we grab our own version from here:

https://sourceforge.net/projects/boost/files/boost/1.75.0/boost_1_75_0.tar.bz2

```sh
cd /home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1
cp ~/boost_1_75_0.tar.bz2 .
tar xvjf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh --with-libraries=filesystem,timer,chrono,program_options,date_time,thread,system,atomic --with-toolset=darwin
./b2 link=static
```

Tell CMake where to find boost:

```sh
export BOOST_ROOT=/home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1/boost_1_75_0

## Build n88util

```sh
cd /home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/n88util.git
cd n88util
git checkout v2.0.0
mkdir ../n88util-build
cd ../n88util-build
cmake -DCMAKE_BUILD_TYPE=Release ../n88util
make
```

Actually, there is no need to actually build it, since we only need the headers (and the CMake configuration). I make it out of habit.

Tell CMake where to find n88util:

```sh
export n88util_DIR=/home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1/n88util-build
```

## Build AimIO

```sh
cd /home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1
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
export AimIO_DIR=/home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1/AimIO-build
```

## Build n88ParaViewPlugins

```sh
cd /home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git checkout v5.9.1
mkdir ../n88ParaViewPlugins-build
cd ../n88ParaViewPlugins-build
export ParaView_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/superbuild/paraview/build
export rkcommon_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/rkcommon-1.5.1/
export openvkl_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/openvkl-0.11.0/
export netCDF_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/netCDF
export nlohmann_json_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/superbuild/nlohmannjson/build/
export openvkl_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/lib/cmake/openvkl-0.11.0/
export PYTHON_DIR=/home/fem/faim-devel/build/ParaViewSuperbuild/v5.9.1/build/install/bin/python3.8
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/fem/faim-devel/build/n88ParaViewPlugins/v5.9.1/build/install ../n88ParaViewPlugins
make
make install
```

If everything worked correctly, the plugins will be in $HOME/code/n88ParaViewPlugins/5.1.2/install .
They can be copied to another system and loaded in ParaView 5.1.2 .
