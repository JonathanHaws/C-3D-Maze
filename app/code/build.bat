
:: Minimal compile script for Windows using clang++
:: If library is not not in local directory its because it is a library from the operating system, compiler, or device driver.

@echo off
set output_path=../
set output_file=maze.exe

if exist %output_file% ( del /F %output_file% )
set global_libs=-lopengl32 -lgdi32 -luser32 -lmsvcrt -lshell32 -DGLEW_STATIC -lglu32 -llibcmt -lole32 -lwinmm -mwindows
set local_libs= libs/imgui/*.cpp -Ilibs/ -llibs/GLFW/glfw3 -lgraphics/opengl/GL/glew32s ../info/windows/metadata.res
clang++ -o %output_path%%output_file% -std=c++17 -I. main.cpp %global_libs% %local_libs% 

cd %output_path% & if exist %output_file% ( start /B %output_file% )
