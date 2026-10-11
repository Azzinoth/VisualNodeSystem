@echo off

REM ============================================================
REM   Run Visual Node System tests on Windows, Linux (WSL) and
REM   Web (Emscripten + Node.js), then print a summary.
REM
REM   Usage: run-tests.bat [debug|release] [windows] [linux] [web]
REM   Default: release, all platforms.
REM   Logs and Results.xml are written to build-tests\<Platform>\.
REM
REM   Linux needs WSL with a distribution that has cmake, ninja,
REM   g++ and the X11/OpenGL headers.
REM   Web needs Emscripten and Ninja, run build-web.bat once to
REM   install them.
REM ============================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-tests.ps1" %*
set "EXIT_CODE=%ERRORLEVEL%"
pause
exit /b %EXIT_CODE%
