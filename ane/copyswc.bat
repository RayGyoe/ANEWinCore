rd /s /q swc
del ANEWinCore.swc

copy /y %pathtome%..\swc\lib\ANEWinCore.swc .

unzip.exe ANEWinCore.swc -d swc

copy /y swc\library.swf .
copy /y library.swf default
copy /y library.swf Windows-x86

rd /s /q swc