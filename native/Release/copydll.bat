SET pathtome=%~dp0


del %pathtome%..\..\example\ext-folder\ANEWinCore.ane\META-INF\ANE\Windows-x86\ANEWinCore.dll
del %pathtome%..\..\example\ext-folder\ANEWinCore.ane\META-INF\ANE\Windows-x86\ANEWinCore.pdb



copy /y ANEWinCore.dll %pathtome%..\..\example\ext-folder\ANEWinCore.ane\META-INF\ANE\Windows-x86\ANEWinCore.dll
copy /y ANEWinCore.dll %pathtome%..\..\example\ext-folder\ANEWinCore.ane\META-INF\ANE\Windows-x86\ANEWinCore.pdb