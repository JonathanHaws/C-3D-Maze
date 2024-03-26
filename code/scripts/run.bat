@echo off
set output_path=../../
set output_file=maze.exe
cd %output_path% & if exist %output_file% ( start /B %output_file% )