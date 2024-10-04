REM Learning how to make life easier developing in C++ with vscode
REM (bat) batch file is basically just a bunch of commands that can automate the process of compiling and running a program.
REM With vscode extension batch runner can turn manually typing in multiple commands into a single f5
REM Also makes it easier to add dependencies to the project like graphics libraries
REM Also nice cause its preinstalled on windows

@echo off

REM Check if the process 'main.exe' is running, then kill it
for /f "tokens=2" %%a in ('tasklist /NH /FI "IMAGENAME eq main.exe"') do taskkill /F /PID %%a

REM Compile the 'main.cpp' source file using Clang++
clang++ maze.cpp -o maze.exe

REM Start the 'main.exe' process
start maze.exe
