#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <Windows.h>
#include <tchar.h>

#include <thread>
#include <winuser.h>
//#include <ShellScalingAPI.h>  //vs140
//#pragma comment(lib, "Shcore.lib")     //���������Ļ������������SetProcessDpiAwareness�޷��������ⲿ����


#include <sstream>

#include <iostream>
#include <string>

using std::string;

#include <vector>
#include<mutex>
#include<condition_variable>
#include<future>
#include<chrono>
/*
#include <windows.h>
#include <string>
#include <memory>
#include <stdio.h>
#include <assert.h>
*/

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")


#include "FlashRuntimeExtensions.h"


extern "C"
{
	__declspec(dllexport) void ANEWinCoreExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);
	__declspec(dllexport) void ANEWinCoreExtFinalizer(void* extData);
}
