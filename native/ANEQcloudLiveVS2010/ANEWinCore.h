#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "FlashRuntimeExtensions.h"

#include "MiniDump.h"

#include <Windows.h>
#include <tchar.h>


#include <winuser.h>
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")     //不加这个库的话，程序会提醒SetProcessDpiAwareness无法解析的外部符号


/*
#include <windows.h>
#include <string>
#include <memory>
#include <stdio.h>
#include <assert.h>
*/


extern "C"
{
	__declspec(dllexport) void ANEWinCoreExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);
	__declspec(dllexport) void ANEWinCoreExtFinalizer(void* extData);
}
