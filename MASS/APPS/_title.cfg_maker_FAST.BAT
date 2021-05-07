@echo off
cd /d %~dp0
set BatchDir=%CD%
:QUICKMODE
FOR %%x in (%*) do (
move %%x %BatchDir%
)
::
mode 120,30
set q=echo ------------------------------------------------------------------------------------------------------------------------
set w=echo ========================================================================================================================

%q%
echo IMPORTANTE# solo funciona con opl beta superior a OPL 1460_DB o OPL 1277 IFCARO
%q%
::timeout 5
cls
echo ========================================================================================================================
title creador de title.cfg masivo
echo este archivo por lotes creara un archivo title.cfg para cada archivo ELF presente en esta carpeta y movera los ELF a la
echo carpeta que le corresponda
echo                                                                                                                       -
echo creado por El_Isra
echo https://github.com/israpps/opl-Title.cfg-maker
echo se recomienda dejar este archivo en la carpeta APPS del opl si juega por USB o SMB para facilitar el proceso
echo ========================================================================================================================
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
echo .
echo .
echo .
echo .
echo ========================================================================================================================
echo ejecucion finalizada.
echo borrando cache
echo ========================================================================================================================

del ISR.ETL
pause
pause
