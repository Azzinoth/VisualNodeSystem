@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM   Build Visual Node System tests for Web (Emscripten + Ninja)
REM   - Installs Emscripten SDK and Ninja inside this repo if
REM     they are missing, then configures, builds, and serves.
REM   - Open the printed link in a browser to run the tests.
REM   - Re-running the script is safe: existing tools are reused.
REM
REM   Usage: build-web.bat [debug|release]   (default: release)
REM   Each config builds into its own folder: build-web\Debug, build-web\Release.
REM
REM   If a previous build is broken, delete build-web\ and rerun.
REM ============================================================

set "CONFIG=Release"
if /i "%~1"=="debug" set "CONFIG=Debug"
if not "%~1"=="" if /i not "%~1"=="debug" if /i not "%~1"=="release" (
    echo Usage: %~nx0 [debug^|release]
    pause & exit /b 1
)

set "REPO_DIR=%~dp0"
if "%REPO_DIR:~-1%"=="\" set "REPO_DIR=%REPO_DIR:~0,-1%"

set "EMSDK_ROOT=%REPO_DIR%\Emscripten_SDK"
set "EMSDK_DIR=%EMSDK_ROOT%\emsdk"
set "NINJA_DIR=%REPO_DIR%\Ninja"
set "BUILD_DIR=%REPO_DIR%\build-web\%CONFIG%"

echo.
echo === Config: %CONFIG%
echo === Repo : %REPO_DIR%
echo === emsdk: %EMSDK_DIR%
echo === ninja: %NINJA_DIR%
echo === build: %BUILD_DIR%
echo.

REM ---------- 1. Emscripten SDK ----------
if exist "%EMSDK_DIR%\emsdk.bat" (
    echo [emsdk] Repo already cloned, skipping clone.
) else (
    echo [emsdk] Cloning emsdk...
    if not exist "%EMSDK_ROOT%" mkdir "%EMSDK_ROOT%"
    pushd "%EMSDK_ROOT%"
    git clone https://github.com/emscripten-core/emsdk.git
    if errorlevel 1 (
        echo [emsdk] ERROR: git clone failed. Is git installed and on PATH?
        popd & pause & exit /b 1
    )
    popd
)

REM Install/activate latest only if not already done.
if exist "%EMSDK_DIR%\upstream\emscripten\emcc.bat" (
    echo [emsdk] Latest already installed and activated, skipping.
) else (
    pushd "%EMSDK_DIR%"
    echo [emsdk] Installing latest...
    call emsdk.bat install latest
    if errorlevel 1 ( echo [emsdk] ERROR: install failed. & popd & pause & exit /b 1 )
    echo [emsdk] Activating latest...
    call emsdk.bat activate latest
    if errorlevel 1 ( echo [emsdk] ERROR: activate failed. & popd & pause & exit /b 1 )
    popd
)

REM Bring emcc, emcmake, etc. onto PATH for this shell.
call "%EMSDK_DIR%\emsdk_env.bat" >nul

REM ---------- 2. Ninja ----------
if exist "%NINJA_DIR%\ninja.exe" (
    echo [ninja] Already present, skipping download.
) else (
    echo [ninja] Downloading ninja-win.zip...
    if not exist "%NINJA_DIR%" mkdir "%NINJA_DIR%"
    pushd "%NINJA_DIR%"
    curl.exe -L -o ninja-win.zip https://github.com/ninja-build/ninja/releases/latest/download/ninja-win.zip
    if errorlevel 1 ( echo [ninja] ERROR: download failed. & popd & pause & exit /b 1 )
    tar -xf ninja-win.zip
    if errorlevel 1 ( echo [ninja] ERROR: extract failed. & popd & pause & exit /b 1 )
    del ninja-win.zip
    popd
)

set "PATH=%NINJA_DIR%;%PATH%"

echo.
echo === Tool versions ===
call emcc --version
call ninja --version
echo.

REM ---------- 3. Configure + Build ----------
REM -S and -B are given explicitly, so the in-source Visual Studio
REM CMakeCache.txt at the repo root does not interfere and is left alone.
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo [cmake] Configuring %CONFIG% (emcmake cmake -G Ninja)...
call emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=%CONFIG% -S "%REPO_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 (
    echo [cmake] ERROR: configure failed.
    echo         If this is a stale cache, delete "%BUILD_DIR%" and rerun.
    pause & exit /b 1
)

echo [build] Building...
cmake --build "%BUILD_DIR%"
if errorlevel 1 ( echo [build] ERROR: build failed. & pause & exit /b 1 )

REM ---------- 4. Serve ----------
echo.
echo ============================================================
echo  Build complete. Serving %BUILD_DIR% on http://localhost:8080
echo  Run all tests: http://localhost:8080/Visual_Node_System_Tests.html
echo  Run a subset:  http://localhost:8080/Visual_Node_System_Tests.html?filter=ExecutionFlowNodesTests.*
echo  Press Ctrl+C to stop.
echo ============================================================
echo.
pushd "%BUILD_DIR%"
python -m http.server 8080
popd

endlocal
