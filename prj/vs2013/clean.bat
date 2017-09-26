rem @echo off
rem Delete everything except 'gnuplot' folder, 'make_sln.bat' and 'clean.bat'.
attrib +r make_sln.bat
attrib +r clean.bat
rmdir CMakeFiles /s /q
rmdir Win32 /s /q
rmdir src /s /q
del *.* /q
attrib -r make_sln.bat
attrib -r clean.bat