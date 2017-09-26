#!/bin/bash
#Delete everything except 'makefile.sh'.
rm -r -f CMakeFiles
rm -r -f src
#find . ! -name 'makefile.sh' -type f -exec rm -f {} +
rm -f Makefile
rm -f Tricycle
rm -f cmake_install.cmake
rm -f CMakeCache.txt
