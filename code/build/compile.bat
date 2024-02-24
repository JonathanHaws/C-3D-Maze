
@echo off

set output_path=..\app\
set output_file=maze.exe

if exist %output_file% ( del /F %output_file% )

cd ../ 
clang++ -o %output_path%%output_file% -std=c++17 -I. -Ilibs -Ilibs/imgui main.cpp libs/imgui/*.cpp -Llibs -Llibs/GL -Llibs/GLFW -lglfw3 -lopengl32 -lgdi32 -luser32 -lmsvcrt -lshell32 -lglew32s -DGLEW_STATIC -lglu32 -llibcmt -lole32 -lwinmm -mwindows

:: If library is not not in local directory its because it is a library from the operating system, compiler, or driver.

cd %output_path% & if exist %output_file% ( start /B %output_file% )
