# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ukasz/esp/esp-idf/components/bootloader/subproject"
  "/home/ukasz/plantie/build/bootloader"
  "/home/ukasz/plantie/build/bootloader-prefix"
  "/home/ukasz/plantie/build/bootloader-prefix/tmp"
  "/home/ukasz/plantie/build/bootloader-prefix/src/bootloader-stamp"
  "/home/ukasz/plantie/build/bootloader-prefix/src"
  "/home/ukasz/plantie/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ukasz/plantie/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ukasz/plantie/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
