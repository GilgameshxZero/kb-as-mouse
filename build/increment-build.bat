@ECHO OFF

REM Increments build version at compile-time.

SETLOCAL ENABLEDELAYEDEXPANSION
SET buildFile=%~dp0../include/build.hpp
IF EXIST "%buildFile%.tmp" DEL "%buildFile%.tmp"
FOR /F "tokens=*" %%R IN (%buildFile%) DO (
	IF "%%R"=="" ECHO. >> "%buildFile%.tmp"
	SET isBuildLine=0
	FOR /F "tokens=1-3 delims= " %%A IN ("%%R") DO (
		IF "%%B"=="KBASMOUSE_VERSION_BUILD" SET isBuildLine=1
		IF !isBuildLine! EQU 1 (
			SET /A buildVersion=%%C+1
			ECHO | SET /P="%%A %%B !buildVersion!">> "%buildFile%.tmp"
			ECHO. >> "%buildFile%.tmp"
			ECHO Build: !buildVersion!.
		) ELSE (
			echo %%R>> "%buildFile%.tmp"
		)
	)
)
MOVE "%buildFile%.tmp" "%buildFile%" >nul
ENDLOCAL
