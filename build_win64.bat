@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@set SDL2_DIR=
@set OUT_DIR=Debug64
@set OUT_EXE=yaciC
@set INCLUDES= /I%SDL2_DIR%\include
@set SOURCES=src\main.c src\chip_vm.c
@set LIBS=/LIBPATH:%SDL2_DIR%\lib\x64 SDL2.lib SDL2main.lib shell32.lib
mkdir %OUT_DIR%
cl /nologo /TC /Zi /DEBUG:FULL /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS% /subsystem:console
