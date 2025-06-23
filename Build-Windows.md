# Build instructions for Windows

These build instructions are particular to ParaView version 5.13.1.
Checkout a different tag of the source code if you want to build for another version of ParaView.

Before building the plugins, it is necessary to build ParaView, using the ParaView superbuild,
in a way such that the plugins built against it will load in the binary distribution of ParaView.

## Learn more about plugins

The C++ documentation contains dedicated pages:
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginHowto.html
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginMigration.html
- https://www.paraview.org/paraview-docs/nightly/cxx/index.html

The source code contains many plugin examples.
- https://gitlab.kitware.com/paraview/paraview/-/tree/master/Examples/Plugins

Note that any documentation on the ParaView wiki is out of date.

## Get MSVC for Windows

To build ParaView plugins on Windows, you need the Microsoft Visual C++ (MSVC) compiler.

1. **Download and install Visual Studio Build Tools:**
   - Go to [Visual Studio Downloads](https://visualstudio.microsoft.com/downloads/).
   - Under "Tools for Visual Studio," download the **Build Tools for Visual Studio**.

2. **Install the required components:**
   - Run the installer.
   - Select the **C++ build tools** workload (called "Desktop development with C++").
   - Make sure to include the latest MSVC compiler, Windows 10/11 SDK, and CMake tools if available.

3. **Finish installation and restart your computer if prompted.**

You do **not** need the full Visual Studio IDE—just the Build Tools are sufficient for command-line builds.

## Download Strawberry Perl

Some ParaView and Boost build steps require Perl. The easiest way to get Perl on Windows is to use Strawberry Perl.

1. **Go to the Strawberry Perl website:**  
   [https://strawberryperl.com/](https://strawberryperl.com/)

2. **Download the latest 64-bit (or 32-bit if needed) installer**  
   - Choose the version that matches your system (most users should use the 64-bit version).

3. **Run the installer**  
   - Accept the default options and complete the installation.

4. **Add Strawberry Perl to your PATH (if not already set)**  
   - The installer usually does this automatically.  
   - To check, open a new Command Prompt and run:  
     ```
     perl --version
     ```
     If you see version information, Perl is installed and on your PATH.

Strawberry Perl is now ready for use in your build environment.

## Download CMake (3.26.4)

To build ParaView plugins, you need CMake. For best results and to use the graphical interface (`cmake-gui`), install the official CMake binary distribution.

**Important:**  
Use **CMake version 3.26.4**. n88ParaViewPlugins will not be compatible with newer CMake versions.

1. **Download version 3.26.4:**  
   **Direct download:**  
   [Download CMake 3.26.4 for Windows x64 (.msi)](https://objects.githubusercontent.com/github-production-release-asset-2e65be/537699/680ff860-ab36-4858-8266-b1dedbff4ebc?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=releaseassetproduction%2F20250617%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20250617T163402Z&X-Amz-Expires=300&X-Amz-Signature=a3be18e60227b05617c200080ea959726a7a3d110babdba2e8346755b8dc619f&X-Amz-SignedHeaders=host&response-content-disposition=attachment%3B%20filename%3Dcmake-3.26.4-windows-x86_64.msi&response-content-type=application%2Foctet-stream)

2. **Run the installer:**  
   - Accept the default options.
   - Make sure to select **"Add CMake to the system PATH for all users"** during installation.
   - This will install both the command-line tools and `cmake-gui`.

3. **Verify the installation:**  
   - Open "x64 Native Tools Command Prompt for VS 2022":
     ```
     cmake --version
     ```
     It should report version 3.26.4.
   - You can also launch the graphical interface by running:
     ```
     cmake-gui
     ```

CMake is now ready for use in your build environment.

**Note** You must open `x64 Native Tools Command Prompt for VS 2022` as `Administator` on Windows for the rest of the build.

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

If you want to build for a specific ParaView version, you need to checkout the specific version before starting any builds.

To see all available versions, run:
```sh
git tag
```

To checkout the specific version (i.e. v5.13.1), run:
```sh
git checkout v5.13.1
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

## Install Python 3.10

**Note**: As of June 2025, it is recommended to build the plugins outside of a Conda environment to avoid conflicts with MSVC.

1. **Go to the official Python downloads page:**  
   [https://www.python.org/downloads/release/python-3100/](https://www.python.org/downloads/release/python-3100/)

2. **Download the Windows installer:**  
   - Choose the "Windows installer (64-bit)" (`python-3.10.x-amd64.exe`) for most systems.

3. **Run the installer:**  
   - Select **"Add Python 3.10 to PATH"** at the bottom of the installer window.
   - Click **"Install Now"** and follow the prompts to complete the installation.

4. **Verify the installation:**  
   - Open a new Command Prompt and run:
     ```
     python --version
     ```
     It should report Python 3.10.x.

Python 3.10 is now ready for use in your build environment.

## Configure CMake

Create build and downloads directories (do this while you are in the paraview-superbuild directory).
The downloads directory is where the superbuild will cache any tarballs it downloads.

The following instructions are for configuring using cmake-gui.

```sh
mkdir ..\paraview-superbuild-build
mkdir ..\paraview-superbuild-downloads
cd ..\paraview-superbuild-build
cmake-gui ..\paraview-superbuild
```

The following settings are recommended:
- Select **Ninja** as the generator
- Set `ENABLE_ospray` to `ON`. This affects which system libraries everything links to (e.g. netcdf, hdf5, etc).
- Set `ENABLE_hdf5` to `ON`.
- Set `ENABLE_netcdf` to `ON`.
- Set `ENABLE_python3` to `ON`.

After applying the settings, press "Configure".

Some further settings that will pop up in subsequent iterations of the configuration process are: 
- Set `BUILD_TESTING` to `OFF`.
- Set `USE_SYSTEM_hdf5` to `OFF` (default).
- Set `USE_SYSTEM_netcdf` to `OFF` (default).
- Set `USE_SYSTEM_python3` to `OFF` (default).

Press "Configure" again, then "Generate".

If and *only* if you want to install the ParaView GUI from the superbuild, enable Qt.  This is not recommended,
since it's much easier to download the ParaView binary package instead.
- Set `ENABLE_qt5` to `ON` (this will add build time and requires Qt!)
- In the shell, `export CMAKE_PREFIX_PATH=/path/to/Qt5.12.12/5.12.12/clang_64`

## Build Paraview-Superbuild

After configuring with cmake-gui, open an `x64 Native Tools Command Prompt for VS 2022` window and set your working directory to the \paraview-superbuild-build directory. **You must run the terminal as an administator**, as the plugins will be installed to `C:\Program Files`.

```sh
ninja install -v
```

This build will take a very long time, please ensure that your Windows computer does not go to sleep.

## Build n88ParaViewPlugins

Choose a location to build n88ParaViewPlugins.  This build is similar to a superbuild,
in that it downloads dependencies like Boost and AimIO automatically.  Ideally, you want
to use the version of n88ParaViewPlugins that matches ParaView itself, but you can try
the master branch if no exact match is available.

```sh
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
mkdir -p ..\n88ParaViewPlugins-build\v5.13.1
cd ..\n88ParaViewPlugins-build\v5.13.1

```
Next, some environment variable must be set, so that cmake can find what it needs from ParaView.
The first environment variable will be the location of the ParaView superbuild.  What I have
written below is just an example.

```shell
set PVSB=C:\Path\To\paraview-superbuild-build

set ParaView_DIR=%PVSB%\superbuild\paraview\build
set rkcommon_DIR=%PVSB%\install\lib\cmake\rkcommon-1.7.0\
set openvkl_DIR=%PVSB%\install\lib\cmake\openvkl-1.0.1\
set netCDF_DIR=%PVSB%\install\lib\cmake\netCDF
set nlohmann_json_DIR=%PVSB%\superbuild\nlohmannjson\build\
set ZLIB_ROOT=%PVSB%\install
```
Now, configure the build with `cmake-gui`. Choose Ninja as your generator.

```sh
cmake-gui ..\..\n88ParaviewPlugins
```

Set `CMAKE_BUILD_TYPE` to `Release`, then hit "Configure" and "Generate".

Finally, run `ninja install -v`.

After the build completes, you can find the plugins here:
- `C:\Program Files\ParaviewPlugins\bin\v5.13` for `x32` bit Windows
- `C:\Program Files\ParaviewPlugins (x86)\bin\v5.13` for `x64` bit Windows

The ParaView version might differ based on your selected `paraview-superbuild` version.

## Create an archive for distribution

Package three plugin folders in `C:\Program Files\ParaviewPlugins\bin\v5.13` or `C:\Program Files\ParaviewPlugins (x86)\bin\v5.13` in a zip file. These folders should contain the following DLL files:
1. AIMReader.dll
2. ImageGaussianSmooth.dll
3. N88ModelReader.dll