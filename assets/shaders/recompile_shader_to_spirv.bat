@echo off
echo Running from: %CD%
setlocal ENABLEDELAYEDEXPANSION

for /F %%a in ('"prompt $E & for %%b in (1) do rem"') do set "ESC=%%a"
set GLSLC="C:/VulkanSDK/1.4.313.2/Bin/glslc.exe"

::Move to shader directory 
cd /d "%~dp0"

set LOG_FILE=spirvs\shader_compilation_report.log
del /q %LOG_FILE% >nul 2>&1

echo [Cleaning old SPIR-V shaders...]
echo.

>> %LOG_FILE% echo.
>> %LOG_FILE% echo [%DATE% %TIME%] Begin Cleaning Old SPIR-V shaders.

for %%f in (spirvs\*.spv) do (
	echo -- Deleting %%~nxf
	>> %LOG_FILE% echo %%~nxf 		-Deleted.
	del "%%f" > nul 2>&1
)

>> %LOG_FILE% echo [%DATE% %TIME%] Completed Cleaning Old SPIR-V shaders.
>> %LOG_FILE% echo.

echo.
echo Done Cleaning old SPIR-V shaders.
echo. 

echo [Recompiling shaders...]
>> %LOG_FILE% echo. 
>> %LOG_FILE% echo [%DATE% %TIME%] Recompiling shaders to SPIR-V.
>> %LOG_FILE% echo. 



set v_fail_count=0
set v_success_count=0
set v_total_shader=0


echo.
echo - Vertex shaders...
echo. 
>> %LOG_FILE% echo [%DATE% %TIME%] == Compiling Vertex shaders to SPIR-V.==

for %%f in (*.vert) do (
	<nul set /p="-- Compiling - %%~nxf to SPIR-V... "
	%GLSLC% "%%f" -o spirvs/"%%~nf".vert.spv 2> spirvs/%%~nf.vert.log

	if !errorlevel! NEQ 0 (
		echo !ESC![1;31m[FAIL]!ESC![0m
		echo [%DATE% %TIME%] [VEXTEX] - %%~nxf 		: FAIL >> %LOG_FILE%
		set /a v_fail_count+=1
	) else (
		echo !ESC![1;32m[OK]!ESC![0m
		echo [%DATE% %TIME%] [VEXTEX] - %%~nxf 		: OK >> %LOG_FILE%
		set /a v_success_count+=1
		del spirvs\%%~nf.vert.log >nul 2>&1
	)
	
	set /a v_total_shader+=1
)



echo.
echo - Fragment shaders...
echo. 

set f_fail_count=0
set f_success_count=0
set f_total_shader=0

>> %LOG_FILE% echo. 
>> %LOG_FILE% echo [%DATE% %TIME%] == Compiling Fragment shaders to SPIR-V.==
for %%f in (*.frag) do (
	<nul set /p="-- Compiling - %%~nxf to SPIR-V... "
	%GLSLC% "%%f" -o spirvs/"%%~nf".frag.spv 2> spirvs/%%~nf.frag.log
	
	if !errorlevel! NEQ 0 (
		echo !ESC![1;31m[FAIL]!ESC![0m
		echo [%DATE% %TIME%] [FRAGMENT] - %%~nxf 	: FAIL >> %LOG_FILE%
		set /a f_fail_count+=1
	) else (
		echo !ESC![1;32m[OK]!ESC![0m
		echo [%DATE% %TIME%] [FRAGMENT] - %%~nxf 		: OK >> %LOG_FILE%
		set /a f_success_count+=1
		del spirvs\%%~nf.frag.log >nul 2>&1
	)
	
	set /a f_total_shader+=1
)


echo.
echo - Compute shaders...
echo. 

set c_fail_count=0
set c_success_count=0
set c_total_shader=0

>> %LOG_FILE% echo. 
>> %LOG_FILE% echo [%DATE% %TIME%] == Compiling Compute shaders to SPIR-V.==
for %%f in (*.comp) do (
	<nul set /p="-- Compiling - %%~nxf to SPIR-V... "
	%GLSLC% "%%f" -o spirvs/"%%~nf".comp.spv 2> spirvs/%%~nf.comp.log
	
	if !errorlevel! NEQ 0 (
		echo !ESC![1;31m[FAIL]!ESC![0m
		echo [%DATE% %TIME%] [COMPUTE] - %%~nxf 	: FAIL >> %LOG_FILE%
		set /a c_fail_count+=1
	) else (
		echo !ESC![1;32m[OK]!ESC![0m
		echo [%DATE% %TIME%] [COMPUTE] - %%~nxf 		: OK >> %LOG_FILE%
		set /a c_success_count+=1
		del spirvs\%%~nf.comp.log >nul 2>&1
	)
	
	set /a c_total_shader+=1
)


echo.
echo - Geometry shaders...
echo. 

set g_fail_count=0
set g_success_count=0
set g_total_shader=0

>> %LOG_FILE% echo. 
>> %LOG_FILE% echo [%DATE% %TIME%] == Compiling Geometry shaders to SPIR-V.==
for %%f in (*.geom) do (
	<nul set /p="-- Compiling - %%~nxf to SPIR-V... "
	%GLSLC% "%%f" -o spirvs/"%%~nf".geom.spv 2> spirvs/%%~nf.geom.log
	
	if !errorlevel! NEQ 0 (
		echo !ESC![1;31m[FAIL]!ESC![0m
		echo [%DATE% %TIME%] [GEOMETRY] - %%~nxf 	: FAIL >> %LOG_FILE%
		set /a g_fail_count+=1
	) else (
		echo !ESC![1;32m[OK]!ESC![0m
		echo [%DATE% %TIME%] [GEOMETRY] - %%~nxf 		: OK >> %LOG_FILE%
		set /a g_success_count+=1
		del spirvs\%%~nf.geom.log >nul 2>&1
	)
	
	set /a g_total_shader+=1
)

:summary
set /a total_success=!v_success_count! + !f_success_count! + !c_success_count! + !g_success_count!
set /a total_fail=!v_fail_count! + !f_fail_count! + !c_fail_count! + !g_fail_count!
set /a total_all=!v_total_shader! + !f_total_shader! + !c_total_shader! + !c_total_shader!

>> %LOG_FILE% echo.
>> %LOG_FILE% echo =============================
>> %LOG_FILE% echo Shader Compilation Summary 
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo Vertex shaders OK	: !v_success_count!
>> %LOG_FILE% echo Vertex shaders FAIL	: !v_fail_count!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo Vertex Total		: !v_total_shader!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo.
>> %LOG_FILE% echo Fragment shaders OK	: !f_success_count!
>> %LOG_FILE% echo Fragment shaders FAIL	: !f_fail_count!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo Fragment Total		: !f_total_shader!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo.
>> %LOG_FILE% echo Compute shaders OK	: !c_success_count!
>> %LOG_FILE% echo Compute shaders FAIL	: !c_fail_count!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo Compute Total		: !c_total_shader!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo.
>> %LOG_FILE% echo Geometry shaders OK	: !g_success_count!
>> %LOG_FILE% echo Geometry shaders FAIL	: !g_fail_count!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo Geometry Total		: !g_total_shader!
>> %LOG_FILE% echo -----------------------------
>> %LOG_FILE% echo.
>> %LOG_FILE% echo Total OK		: !total_success!
>> %LOG_FILE% echo Total FAIL		: !total_fail!
>> %LOG_FILE% echo Total SHADERS		: !total_all!
>> %LOG_FILE% echo =============================
>> %LOG_FILE% echo.

echo.
echo ==========================
echo Shader Compilation Summary
echo --------------------------
echo Successful 	: %total_success%
echo Failed	    	: %total_fail%
echo Total	    	: %total_all%
echo ==========================
echo.

if %total_fail% NEQ 0 (
	
	echo Some shaders failed . Check spirvs/*.log for details.
	>> %LOG_FILE% echo Some shaders failed . Check spirvs/*.log for details.
	>> %LOG_FILE% echo.
	
	echo.
	echo !ESC![1;31mShowing content of Failed Shaders...!ESC![0m
	echo.
	
	echo FAILED VERTEX SHADERS
	echo.
	
	>> %LOG_FILE% echo Check following logs for information:
	>> %LOG_FILE% echo 	failed vertex shaders:
	
	for %%f in (spirvs\*.vert.log) do (
		echo !ESC![1;31m-- %%~nxf LOG --!ESC![0m
		type "%%f"
		echo.
	
		>> %LOG_FILE% echo 		"%%f"
	)
	
	>> %LOG_FILE% echo 	failed fragment shaders:
	echo FAILED FRAGMENT SHADERS
	echo.
	
	for %%f in (spirvs\*.frag.log) do (
		echo !ESC![1;31m-- %%~nxf LOG --!ESC![0m
		type "%%f"
		echo.
	
	>> %LOG_FILE% echo 		"%%f"
	)
	
	>> %LOG_FILE% echo 	failed compute shaders:
	echo FAILED COMPUTE SHADERS
	echo.
	
	for %%f in (spirvs\*.comp.log) do (
		echo !ESC![1;31m-- %%~nxf LOG --!ESC![0m
		type "%%f"
		echo.
	
	>> %LOG_FILE% echo 		"%%f"
	)
	
	>> %LOG_FILE% echo 	failed geometry shaders:
	echo FAILED GEOMETRY SHADERS
	echo.
	
	for %%f in (spirvs\*.geom.log) do (
		echo !ESC![1;31m-- %%~nxf LOG --!ESC![0m
		type "%%f"
		echo.
	
	>> %LOG_FILE% echo 		"%%f"
	)
	
) else (
	echo !ESC![1;32mSUCCESSFULLY COMPILED ALL SHADERS TO SPIR-V.!ESC![0m
	>> %LOG_FILE% echo SUCCESSFULLY COMPILED ALL SHADERS TO SPIR-V.
)


pause
if %errorlevel% neq 0 exit /b 1