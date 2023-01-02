@echo off

if exist "bin" (
    RD /S /Q "bin"
)

MD "bin"

::clang -o bin\decoder.exe -Iinclude src\*.c -L ..\include\SDL2-2.26.0\lib\x64\ -lSDL2
clang.exe -Iinclude -I SDL2-2.26.0\include\ -o bin\decoder.exe decoder\*.c src\*.c -L SDL2-2.26.0\lib\x64\ -lSDL2