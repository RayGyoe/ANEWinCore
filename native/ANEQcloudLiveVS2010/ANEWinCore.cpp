#include "ANEWinCore.h"
#include <sstream>

std::string intToStdString(int value)
{
	std::stringstream str_stream;
	str_stream << value;
	std::string str = str_stream.str();

	return str;
}

extern "C" {

	const char *TAG = "ANEWinCore:";
	

	bool isCrateCrashDump = false;

	//初始化
	FREObject isSupported(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "isSupport");

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}

	//崩溃监控
	FREObject crashDump(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "crashDump");

		if (isCrateCrashDump == false) {
			MiniDump::EnableAutoDump(true);
			printf("\n%s,%s", TAG, "crashDump EnableAutoDump");
		}
		isCrateCrashDump = true;

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}


	//强制关闭程序
	FREObject killProcess(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "killProcess");
		TerminateProcess(GetCurrentProcess(), 0);

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}


	//禁用休眠，屏保
	FREObject keepScreenOn(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "keepScreenOn");

		uint32_t result = 0;
		auto ret = false;
		FREGetObjectAsBool(argv[0], &result);
		if (result > 0) ret = true;

		if (ret) {
			//禁止屏幕保护
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, false, 0, 0);
			//禁止电源管理，避免睡眠、待机
			SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
		}
		else {
			//恢复屏幕保护
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, true, 0, 0);
			//恢复电源管理
			SetThreadExecutionState(ES_CONTINUOUS);
		}

		return NULL;
	}

	FREObject getScreenSize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "getScreenSize");


		HDC desktopDc = GetDC(NULL);
		//// Get native resolution  
		int horizontalDPI = GetDeviceCaps(desktopDc, LOGPIXELSX);
		int verticalDPI = GetDeviceCaps(desktopDc, LOGPIXELSY);

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		printf("\n%s%s   = width:%i   === height:%i\n", TAG, "getScreenShareSize", width, height);

		std::string rect = intToStdString(width) + "||" + intToStdString(height) + "||" + intToStdString(horizontalDPI) + "||" + intToStdString(verticalDPI);

		FREObject result;
		auto status = FRENewObjectFromUTF8(uint32_t(strlen(rect.c_str())) + 1, reinterpret_cast<const uint8_t *>(rect.c_str()), &result);
		return result;
	}



	///
	// Flash Native Extensions stuff	
	void ANEWinCoreContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet) {

		static FRENamedFunction extensionFunctions[] =
		{
			{ (const uint8_t*) "isSupported",     NULL, &isSupported },

			{ (const uint8_t*) "crashDump",     NULL, &crashDump },

			{ (const uint8_t*) "killProcess",     NULL, &killProcess },

			{ (const uint8_t*) "keepScreenOn",     NULL, &keepScreenOn },

			{ (const uint8_t*) "getScreenSize",     NULL, &getScreenSize },
			
		};

		*numFunctionsToSet = sizeof(extensionFunctions) / sizeof(FRENamedFunction);
		*functionsToSet = extensionFunctions;
	}

	void ANEWinCoreContextFinalizer(void * extData)
	{
		printf("\n%s,%s", TAG, "ANEWinCoreContextFinalizer：release()");
	}


	void ANEWinCoreExtInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
	{
		*ctxInitializer = &ANEWinCoreContextInitializer;
		*ctxFinalizer = &ANEWinCoreContextFinalizer;
	}

	void ANEWinCoreExtFinalizer(void* extData)
	{
		return;
	}
}

