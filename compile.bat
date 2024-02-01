
@echo off

set output_file=main.exe

if exist %output_file% ( del /F %output_file% )

clang++ -o app\%output_file% -std=c++17 -Icode -Ilibs -Ilibs/imgui code/main.cpp libs/imgui/*.cpp -Llibs -Llibs/GL -Llibs/GLFW -lglfw3 -lopengl32 -lgdi32 -luser32 -lmsvcrt -lshell32 -lglew32s -DGLEW_STATIC -lglu32 -llibcmt -lole32

:: If library is not not in local directory its because it is a library from the operating system, compiler, or driver.

cd app & if exist %output_file% ( start /B %output_file% )
