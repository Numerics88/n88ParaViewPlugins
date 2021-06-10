#!/bin/bash
#
# This script must be run from directory containing the library or executable to modify.
#
# It is typically run like this:
#
# cd /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins-build/lib/paraview-5.9/plugins/ImageGaussianSmooth
# /Volumes/CaseSensitive/build/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/fix_osx_libraries.sh \
#	  /Volumes/CaseSensitive/build/ParaViewSuperbuild/v5.9.1/build \
#   ImageGaussianSmooth.so
#

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
replace_path "$paraviewBuildPath"/superbuild/paraview/build/lib "@executable_path/../Libraries/"
replace_path "$paraviewBuildPath"/install/lib "@executable_path/../Libraries/"
replace_path "@rpath" "@executable_path/../Libraries/"
# replace_path "$PWD" "@loader_path/"

# tbb
replace_path "$paraviewBuildPath"/install/lib "@executable_path/../Libraries/"

# HDF5 libraries
install_name_tool -change libnetcdf.15.dylib @executable_path/../Libraries/libnetcdf.15.dylib "$libpath"
install_name_tool -change libhdf5_hl.200.dylib @executable_path/../Libraries/libhdf5_hl.200.dylib "$libpath"
install_name_tool -change libhdf5.200.dylib @executable_path/../Libraries/libhdf5.200.dylib "$libpath"

