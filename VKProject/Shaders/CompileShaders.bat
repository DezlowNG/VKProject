@echo off
set /p vert="Vertex shader: "
set /p frag="Fragment shader: "
glslangValidator.exe -V %vert%
glslangValidator.exe -V %frag%
pause