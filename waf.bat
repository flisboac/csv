@echo off
setlocal
if "%PYTHON%"=="" set PYTHON=python
%PYTHON% -x "%~dp0waf" %* & exit /b
endlocal
