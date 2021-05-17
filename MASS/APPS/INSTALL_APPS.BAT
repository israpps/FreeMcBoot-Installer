@echo off
cd /d %~dp0
set BatchDir=%CD%
:QUICKMODE
FOR %%x in (%*) do (
move %%x %BatchDir%
)
::
mode 120,30
echo title.cfg maker by matias israelson
set q=echo ------------------------------------------------------------------------------------------------------------------------
set w=echo ========================================================================================================================
::timeout 10
:GET_ELF
echo checking for ELF files...
dir /b /o:n *.ELF >ISR.ETL
set dea=true
for /f "delims=" %%a in (ISR.ETL) do (
::timeout /t 2 >nul
echo 																															-
echo procesando [%%a]
IF NOT EXIST "%CD%\%%~na\title.cfg" (
																	MD "%%~na"
																	Cd "%%~na"
																		(
																		Echo title=%%~na
																		Echo boot=%%a
																		)>title.cfg
																	Cd..
																	Move "%%a" "%CD%\%%~na\%%a"
															) ELSE (
															echo error.
															echo ya existe una aplicacion llamada [%%a]
															)
)
::LOOP###############################################################################################################
del ISR.ETL
