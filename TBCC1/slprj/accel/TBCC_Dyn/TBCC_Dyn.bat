@echo off
set "VSCMD_START_DIR=%CD%"
call "D:\vs\vsIDE\VC\Auxiliary\Build\VCVARSALL.BAT " amd64

cd .
nmake -f TBCC_Dyn.mk 
@if errorlevel 1 goto error_exit
exit 0

:error_exit
echo The make command returned an error of %errorlevel%
exit 1
