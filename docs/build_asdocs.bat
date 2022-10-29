REM Get the path to the script and trim to get the directory.
@echo off

SET AIRSDK=D:\Software\Adobe\AIRSDK\AIR33.1.1.929
SET AIRSDK=D:\Adobe\Flex_Sdk\AIR33.1.1.929


SET projectName=ANEWinCore
echo Setting path to current directory to:
SET pathtome=%~dp0
echo %pathtome%

echo build asdocs
call asdoc ^
-doc-sources %pathtome%..\swc\src ^
-source-path %pathtome%..\swc\src ^
-window-title "Ray.lei %projectName%" ^
-main-title "Ray.lei %projectName% Documentation" ^
-output %pathtome%\asdocs ^
-lenient ^
-library-path+=%AIRSDK%\frameworks\libs\air\airglobal.swc

call asdoc ^
-doc-sources %pathtome%..\swc\src ^
-source-path %pathtome%..\swc\src ^
-window-title "Ray.lei %projectName%" ^
-main-title "Ray.lei %projectName% Documentation" ^
-output %pathtome%\asdocs\tmp ^
-lenient -keep-xml=true -skip-xsl=true ^
-library-path+=%AIRSDK%\frameworks\libs\air\airglobal.swc