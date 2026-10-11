# Runs Visual Node System tests on Windows, Linux (through WSL) and Web (Emscripten + Node.js), then prints a summary.
# Started by run-tests.bat, see it for usage.
param([Parameter(ValueFromRemainingArguments = $true)] [string[]] $Arguments)

$ErrorActionPreference = "Continue"
$RepoDir = $PSScriptRoot
$OutputRoot = Join-Path $RepoDir "build-tests"
$Usage = "Usage: run-tests.bat [debug|release] [windows] [linux] [web]"

$Config = "Release"
$Platforms = @()
foreach ($Argument in $Arguments)
{
	switch ($Argument.ToLower())
	{
		"debug"   { $Config = "Debug" }
		"release" { $Config = "Release" }
		"windows" { $Platforms += "Windows" }
		"linux"   { $Platforms += "Linux" }
		"web"     { $Platforms += "Web" }
		default   { Write-Host "Unknown argument: $Argument"; Write-Host $Usage; exit 1 }
	}
}
if ($Platforms.Count -eq 0) { $Platforms = @("Windows", "Linux", "Web") }

# Runs a command and appends everything it prints to a log file, returns its exit code.
function Invoke-Logged([string] $LogPath, [scriptblock] $Command)
{
	& $Command 2>&1 | ForEach-Object { "$_" } | Out-File -Append -Encoding utf8 $LogPath
	return $LASTEXITCODE
}

# Reads the summary that googletest prints at the end of a run.
function Get-TestSummary([string] $LogPath)
{
	$Summary = @{ Ran = 0; Passed = 0; Failed = @() }
	if (-not (Test-Path $LogPath)) { return $Summary }

	foreach ($Line in Get-Content $LogPath)
	{
		if ($Line -match '^\[==========\] (\d+) tests? from .* ran') { $Summary.Ran = [int]$Matches[1] }
		elseif ($Line -match '^\[  PASSED  \] (\d+) tests?') { $Summary.Passed = [int]$Matches[1] }
		# The list of failed tests at the end has no timing after the test name.
		elseif ($Line -match '^\[  FAILED  \] (\S+\.\S+)$') { $Summary.Failed += $Matches[1] }
	}
	return $Summary
}

function New-Result([string] $Status, [string] $Details, [string] $LogPath = "")
{
	return @{ Status = $Status; Details = $Details; LogPath = $LogPath; Failed = @() }
}

function Get-RunResult([int] $ExitCode, [string] $TestLog)
{
	$Summary = Get-TestSummary $TestLog
	if ($ExitCode -eq 0 -and $Summary.Ran -gt 0 -and $Summary.Failed.Count -eq 0)
	{
		return New-Result "PASSED" "$($Summary.Passed) tests" $TestLog
	}

	$Result = New-Result "FAILED" "$($Summary.Failed.Count) of $($Summary.Ran) tests failed (exit code $ExitCode)" $TestLog
	$Result.Failed = $Summary.Failed
	return $Result
}

function Invoke-WindowsTests
{
	$Dir = Join-Path $OutputRoot "Windows"
	$BuildDir = Join-Path $Dir "build"
	$RunDir = Join-Path $Dir "run"
	$BuildLog = Join-Path $Dir "build.log"
	$TestLog = Join-Path $Dir "test.log"
	New-Item -ItemType Directory -Force $Dir | Out-Null
	Remove-Item $BuildLog, $TestLog -ErrorAction SilentlyContinue

	if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { return New-Result "SKIPPED" "CMake is not on PATH." }

	Write-Host "[Windows] Configuring and building $Config..."
	$ExitCode = Invoke-Logged $BuildLog { cmake -S $RepoDir -B $BuildDir }
	if ($ExitCode -eq 0) { $ExitCode = Invoke-Logged $BuildLog { cmake --build $BuildDir --config $Config -- /m /v:minimal } }
	if ($ExitCode -ne 0) { return New-Result "BUILD FAILED" "see build log" $BuildLog }

	Write-Host "[Windows] Running tests..."
	if (Test-Path $RunDir) { Remove-Item -Recurse -Force $RunDir }
	New-Item -ItemType Directory -Force $RunDir | Out-Null
	Copy-Item -Recurse (Join-Path $RepoDir "Resources") (Join-Path $RunDir "Resources")
	Push-Location $RunDir
	$ExitCode = Invoke-Logged $TestLog { & (Join-Path $BuildDir "$Config\Visual_Node_System_Tests.exe") }
	Pop-Location
	Copy-Item (Join-Path $RunDir "Results.xml") $Dir -ErrorAction SilentlyContinue

	return Get-RunResult $ExitCode $TestLog
}

# Finds a WSL distribution that can build the tests, Docker Desktop's own distributions are ignored.
function Find-WslDistribution
{
	if (-not (Get-Command wsl.exe -ErrorAction SilentlyContinue)) { return @{ Error = "WSL is not installed, install it with: wsl --install" } }

	$env:WSL_UTF8 = "1"
	$Names = @(& wsl.exe --list --quiet 2>$null | ForEach-Object { ($_ -replace "`0", "").Trim() } | Where-Object { $_ -and $_ -notlike "docker-desktop*" })
	if ($LASTEXITCODE -ne 0 -or $Names.Count -eq 0) { return @{ Error = "WSL has no Linux distribution, install one with: wsl --install -d Ubuntu-24.04" } }

	foreach ($Name in $Names)
	{
		& wsl.exe -d $Name --exec sh -c "command -v cmake && command -v ninja && command -v g++" *> $null
		if ($LASTEXITCODE -eq 0) { return @{ Name = $Name } }
	}

	# None has the tools, run-tests-linux.sh will report what is missing.
	return @{ Name = $Names[0] }
}

function Invoke-LinuxTests
{
	$Dir = Join-Path $OutputRoot "Linux"
	$TestLog = Join-Path $Dir "test.log"
	New-Item -ItemType Directory -Force $Dir | Out-Null
	Remove-Item (Join-Path $Dir "build.log"), $TestLog -ErrorAction SilentlyContinue

	$Distribution = Find-WslDistribution
	if ($Distribution.Error) { return New-Result "SKIPPED" $Distribution.Error }

	Write-Host "[Linux] Using WSL distribution $($Distribution.Name)"
	$ScriptPath = & wsl.exe -d $Distribution.Name --exec wslpath -a (Join-Path $RepoDir "run-tests-linux.sh")
	& wsl.exe -d $Distribution.Name --exec bash $ScriptPath $Config 2>&1 | ForEach-Object { Write-Host "[Linux] $_" }
	$ExitCode = $LASTEXITCODE

	if ($ExitCode -eq 2) { return New-Result "SKIPPED" "$($Distribution.Name) is missing tools, see the message above." }
	if ($ExitCode -eq 3) { return New-Result "BUILD FAILED" "see build log" (Join-Path $Dir "build.log") }
	return Get-RunResult $ExitCode $TestLog
}

function Invoke-WebTests
{
	$Dir = Join-Path $OutputRoot "Web"
	$BuildDir = Join-Path $RepoDir "build-web\$Config"
	$BuildLog = Join-Path $Dir "build.log"
	$TestLog = Join-Path $Dir "test.log"
	New-Item -ItemType Directory -Force $Dir | Out-Null
	Remove-Item $BuildLog, $TestLog -ErrorAction SilentlyContinue

	$EmsdkEnv = Join-Path $RepoDir "Emscripten_SDK\emsdk\emsdk_env.bat"
	$NinjaDir = Join-Path $RepoDir "Ninja"
	if (-not (Test-Path $EmsdkEnv) -or -not (Test-Path (Join-Path $NinjaDir "ninja.exe")))
	{
		return New-Result "SKIPPED" "Emscripten or Ninja is not installed, run build-web.bat once to install them."
	}

	# emsdk_env.bat only sets up a cmd session, so the build and the test run happen in one generated cmd script.
	# The build directory is shared with build-web.bat. Node.js comes with Emscripten (EMSDK_NODE).
	$CmdPath = Join-Path $Dir "run-web.cmd"
	@(
		"@echo off",
		"set EMSDK_QUIET=1",
		"set `"PATH=$NinjaDir;%PATH%`"",
		"call `"$EmsdkEnv`" >nul",
		"call emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=$Config -S `"$RepoDir`" -B `"$BuildDir`" >> `"$BuildLog`" 2>&1 || exit /b 3",
		"cmake --build `"$BuildDir`" >> `"$BuildLog`" 2>&1 || exit /b 3",
		"cd /d `"$BuildDir`"",
		"`"%EMSDK_NODE%`" Visual_Node_System_Tests.js > `"$TestLog`" 2>&1",
		"exit /b %ERRORLEVEL%"
	) | Out-File -Encoding ascii $CmdPath

	Write-Host "[Web] Configuring and building $Config, then running tests in Node.js..."
	& cmd.exe /c $CmdPath
	$ExitCode = $LASTEXITCODE

	if ($ExitCode -eq 3) { return New-Result "BUILD FAILED" "see build log" $BuildLog }
	return Get-RunResult $ExitCode $TestLog
}

$Results = [ordered]@{}
foreach ($Platform in $Platforms)
{
	Write-Host ""
	$Timer = [Diagnostics.Stopwatch]::StartNew()
	$Result = & "Invoke-$($Platform)Tests"
	$Result.Seconds = [math]::Round($Timer.Elapsed.TotalSeconds)
	$Results[$Platform] = $Result
}

Write-Host ""
Write-Host "============================================================"
Write-Host " Test results ($Config)"
Write-Host "============================================================"
$bAnyFailed = $false
foreach ($Platform in $Results.Keys)
{
	$Result = $Results[$Platform]
	$Color = switch ($Result.Status) { "PASSED" { "Green" } "SKIPPED" { "Yellow" } default { "Red" } }
	if ($Result.Status -ne "PASSED" -and $Result.Status -ne "SKIPPED") { $bAnyFailed = $true }

	Write-Host (" {0,-8} " -f $Platform) -NoNewline
	Write-Host ("{0,-13}" -f $Result.Status) -ForegroundColor $Color -NoNewline
	Write-Host (" {0}  ({1} s)" -f $Result.Details, $Result.Seconds)
	foreach ($TestName in $Result.Failed) { Write-Host "            $TestName" -ForegroundColor Red }
	if ($Result.Status -ne "PASSED" -and $Result.LogPath) { Write-Host "            Log: $($Result.LogPath)" }
}
Write-Host "============================================================"
Write-Host " Logs and Results.xml: $OutputRoot"

if ($bAnyFailed) { exit 1 }
exit 0
