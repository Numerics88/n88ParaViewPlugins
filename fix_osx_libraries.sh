#!/bin/bash
#
# This script must be run from directory containing the library or executable to modify.
#
# it is typically run like this:
#
# ~/code/n88ParaViewPlugins/5.1.2/n88ParaViewPlugins/fix_osx_libraries.sh /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.1.2/build libImageGaussianSmooth.dylib

paraviewBuildPath="$1"
libpath="$2"

install_name_tool -id "$libpath" "$libpath"

replace_path () {
    stripString="$1"
    insertString="$2"
    echo "Stripping     : $stripString"
    echo "in            : $libpath"
    echo "insert String : $insertString"

    for myfile in `otool -L $libpath | grep "$stripString" | awk '{print $1}'`
    do
       lib=`echo $myfile | awk -F / '{print $NF}'`
       echo "changing $myfile to $insertString$lib" 
       install_name_tool -change "$myfile" "$insertString$lib" "$libpath"
    done
}

# Qt
stripString="$paraviewBuildPath"/install/lib
replaceString="@executable_path/../Frameworks"
for myfile in `otool -L $libpath | grep "$stripString" | grep framework | awk '{print $1}'`
do
   lib=`echo $myfile | sed "s%$stripString%$replaceString%"`
   echo "changing $myfile to $lib" 
   install_name_tool -change "$myfile" "$lib" "$libpath"
done

# Generic dylibs
replace_path "$paraviewBuildPath"/paraview/src/paraview-build/lib "@executable_path/../Libraries/"
replace_path "$paraviewBuildPath"/install/lib "@executable_path/../Libraries/"
replace_path "@rpath" "@executable_path/../Libraries/"
# replace_path "$PWD" "@loader_path/"

# tbb
replace_path "$paraviewBuildPath"/tbb/src/tbb/lib "@executable_path/../Libraries/"

# # For some reason, the HDF5 libraries aren't linked with the path at all:
# install_name_tool -change libhdf5_hl.7.3.0.dylib @executable_path/../Libraries/libhdf5_hl.1.8.9.dylib "$libpath"
# install_name_tool -change libhdf5.7.3.0.dylib @executable_path/../Libraries/libhdf5.1.8.9.dylib "$libpath"
