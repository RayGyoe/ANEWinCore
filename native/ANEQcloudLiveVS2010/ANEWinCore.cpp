#include "ANEWinCore.h"
#include <sstream>

#include <vector>
using std::string;

#include "IEProxy.h"
using namespace ie_proxy;

std::string intToStdString(int value)
{
	std::stringstream str_stream;
	str_stream << value;
	std::string str = str_stream.str();

	return str;
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;

	std::vector<wchar_t> buff(s.size());
	std::locale loc("zh-CN");
	wchar_t* pwszNext = nullptr;
	const char* pszNext = nullptr;
	mbstate_t state = {};
	int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
		(loc).in(state,
			s.data(), s.data() + s.size(), pszNext,
			buff.data(), buff.data() + buff.size(), pwszNext);

	if (std::codecvt_base::ok == res)
	{
		return std::wstring(buff.data(), pwszNext);
	}

	return NULL;
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


	//设置代理
	FREObject setProxyConfig(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		uint32_t string1Length;
		const uint8_t *val;
		auto status = FREGetObjectAsUTF8(argv[0], &string1Length, &val);
		std::string url = std::string(val, val + string1Length);

		printf("\n%s,%s = %s", TAG, "setProxyConfig",url.c_str());
		
		ProxyConfig pc;
		pc.proxy_server = s2ws(url);
		setProxyConfig(&pc);
		

		return NULL;
	}

	FREObject getProxyConfig(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		ProxyConfig pc;
		getProxyConfig(&pc);
		printf("\n%s,%s = %ws", TAG, "getProxyConfig",pc.proxy_server.c_str());
		
		return NULL;
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


			{ (const uint8_t*) "setProxyConfig",     NULL, &setProxyConfig },
			{ (const uint8_t*) "getProxyConfig",     NULL, &getProxyConfig },
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

