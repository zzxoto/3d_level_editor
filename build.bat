@echo off

IF "%VCINSTALLDIR%"=="" (
  pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build"
  call vcvarsall.bat x64
  popd
) ELSE (
  REM skip me
)

if not exist build mkdir build

set commonCompilerOpts=-Zi -nologo -FC -EHa

set code=%cd%\platform_win32.cpp %cd%\editor.cpp %cd%\zzxoto_gl.cpp %cd%\zzxoto_camera.cpp

pushd build
cl %code% %commonCompilerOpts% -Femain user32.lib opengl32.lib gdi32.lib Winmm.lib
popd
