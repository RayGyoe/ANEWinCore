del Windows-x86\ANEWinCore.dll
del Windows-x86-64\ANEWinCore.dll

copy /y %pathtome%..\native\x64\Release\ANEWinCore.dll "./Windows-x86-64"
copy /y %pathtome%..\native\Release\ANEWinCore.dll "./Windows-x86"