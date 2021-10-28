#include "ANEWinCore.h"


#include "IEProxy.h"
using namespace ie_proxy;

#include "CustomURLProtocolApp.h"


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

bool isFREResultOK(FREResult errorCode, std::string errorMessage) {
	if (FRE_OK == errorCode) return true;
	printf("isFREResultOK = %s", errorMessage.c_str());
	return false;
}


std::string getFREString(FREObject value)
{
	uint32_t string1Length;
	const uint8_t *val;
	auto status = FREGetObjectAsUTF8(value, &string1Length, &val);
	return std::string(val, val + string1Length);
}

int32_t getInt32(FREObject freObject) {
	int32_t result = 0;
	auto status = FREGetObjectAsInt32(freObject, &result);
	isFREResultOK(status, "Could not convert FREObject to int32_t.");
	return result;
}


std::wstring UTF82Wide(const std::string& strUTF8)
{
	int nWide = ::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), strUTF8.size(), NULL, 0);

	std::unique_ptr<wchar_t[]> buffer(new wchar_t[nWide + 1]);
	if (!buffer)
	{
		return L"";
	}

	::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), strUTF8.size(), buffer.get(), nWide);
	buffer[nWide] = L'\0';

	return buffer.get();
}
std::string ws2s(const std::wstring &ws)
{
	size_t i;
	std::string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const wchar_t* _source = ws.c_str();
	size_t _dsize = 2 * ws.size() + 1;
	char* _dest = new char[_dsize];
	memset(_dest, 0x0, _dsize);
	wcstombs_s(&i, _dest, _dsize, _source, _dsize);
	std::string result = _dest;
	delete[] _dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

extern "C" {

	const char *TAG = "ANEWinCore:";
	

	bool isCrateCrashDump = false;

	CustomURLProtocol m_CustomURLProtocol;
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


	FREObject createURLProtocol(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		std::string szProtocolName = getFREString(argv[0]);
		std::string szAppPath = getFREString(argv[1]);
		std::string szCompanyName = getFREString(argv[2]);

		printf("\n%s,%s = %s  = %s  = %s", TAG, "createURLProtocol", szProtocolName.c_str(), szAppPath.c_str() , szCompanyName.c_str());

		
		m_CustomURLProtocol.setProtocolName(s2ws(szProtocolName));
		m_CustomURLProtocol.setCompanyName(s2ws(szCompanyName));
		m_CustomURLProtocol.setAppPath(s2ws(szAppPath));

		m_CustomURLProtocol.DeleteCustomProtocol();

		FREObject result;
		auto status = FRENewObjectFromBool(m_CustomURLProtocol.CreateCustomProtocol() == ERROR_SUCCESS, &result);
		return result;
	}

	FREObject existURLProtocol(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		uint32_t string1Length;
		const uint8_t *val;
		FREGetObjectAsUTF8(argv[0], &string1Length, &val);
		std::string szProtocolName = std::string(val, val + string1Length);

		printf("\n%s,%s = %s", TAG, "existURLProtocol", szProtocolName.c_str());

		m_CustomURLProtocol.setProtocolName(s2ws(szProtocolName));

		int keyType = getInt32(argv[1]);

		FREObject result;
		auto status = FRENewObjectFromBool(m_CustomURLProtocol.existCustomProtocol(keyType), &result);
		return result;
	}


	FREObject clearURLProtocol(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		uint32_t string1Length;
		const uint8_t *val;
		FREGetObjectAsUTF8(argv[0], &string1Length, &val);
		std::string szProtocolName = std::string(val, val + string1Length);

		printf("\n%s,%s = %s", TAG, "clearURLProtocol", szProtocolName.c_str());

		m_CustomURLProtocol.setProtocolName(s2ws(szProtocolName));

		FREObject result;
		auto status = FRENewObjectFromBool(m_CustomURLProtocol.DeleteCustomProtocol() == ERROR_SUCCESS, &result);
		return result;
	}

	
	BOOL IsRunasAdmin()
	{
		BOOL bElevated = FALSE;
		HANDLE hToken = NULL;

		// Get current process token
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			return FALSE;

		TOKEN_ELEVATION tokenEle;
		DWORD dwRetLen = 0;

		// Retrieve token elevation information
		if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
		{
			if (dwRetLen == sizeof(tokenEle))
			{
				bElevated = tokenEle.TokenIsElevated;
			}
		}

		CloseHandle(hToken);
		return bElevated;
	}

	FREObject isAdminRun(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		FREObject result;
		auto status = FRENewObjectFromBool(IsRunasAdmin()==TRUE, &result);
		return result;
	}


	FREObject getProcessHWnds(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{

		std::string hwnds = "";
		////



		/////
		FREObject result;
		auto status = FRENewObjectFromUTF8(uint32_t(strlen(hwnds.c_str())) + 1, reinterpret_cast<const uint8_t *>(hwnds.c_str()), &result);
		return result;
	}

	/*
	FREObject setProcessDpiAwareness(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		//
		//SetProcessDpiAwarenessContext();
		
		int awareness = getInt32(argv[0]);
		SetProcessDpiAwareness((PROCESS_DPI_AWARENESS)awareness);
		//SetProcessDPIAware();

		printf("\nsetProcessDpiAwareness=%i\n", awareness);

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}*/



	int f(FREObject AirClass, std::string &funname)
	{
		std::cout << "start" << std::endl;
		std::cout << "this thread id = " << std::this_thread::get_id() << std::endl;
		std::cout << "end" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		FREObject  re;
		FREResult es = FRECallObjectMethod(AirClass, reinterpret_cast<const uint8_t *>(funname.data()), 0, NULL, &re, NULL);
		printf("\n runCoroutine = result=%i\n", es);

		//std::this_thread::yield();

		return 100;
	}


	FREObject runCoroutine(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n runCoroutine = threadId=%i\n", GetCurrentThreadId());

		std::string funname = getFREString(argv[1]);
		FREObject AirClass = argv[0];
		std::thread run([&](FREObject AirClass,std::string &funname) {
			printf("\n segmentAI GetCurrentThreadId %d\n", GetCurrentThreadId());
			
			FREObject  re;
			FREResult es = FRECallObjectMethod(AirClass, reinterpret_cast<const uint8_t *>(funname.data()), 0, NULL, &re, NULL);
			printf("\n runCoroutine = result=%i\n", es);
		}, AirClass,funname);
		run.detach();


		//std::future<int> fu = std::async(f, argv[0], funname);
		//std::cout << fu.get() << std::endl;


		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}

	// 运行外部exe
	FREObject runExec(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n runExec = threadId=%i\n", GetCurrentThreadId());

		std::string runString = ws2s(UTF82Wide(getFREString(argv[0])));
		int type = getInt32(argv[1]);
		printf("\n runExec = %s\n", runString.c_str());

		LPCSTR path = runString.c_str();
		

		switch (type)
		{
			case 24:
				WinExec(path, WM_SHOWWINDOW);
				break;
			default:
				WinExec(path, (UINT)type);
				break;
		}
		


		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}

	// 开启对话框Per-Monitor DPI Aware支持(至少Win10)
	inline BOOL EnablePerMonitorDialogScaling()
	{
		typedef BOOL(WINAPI *PFN_EnablePerMonitorDialogScaling)();
		PFN_EnablePerMonitorDialogScaling pEnablePerMonitorDialogScaling = (PFN_EnablePerMonitorDialogScaling)GetProcAddress(GetModuleHandleW(L"user32.dll"), (LPCSTR)2577);
		if (pEnablePerMonitorDialogScaling) return pEnablePerMonitorDialogScaling();
		return false;
	}

	// 开启子窗体DPI消息(至少Win10)
	inline BOOL EnableChildWindowDpiMessage(
		_In_ HWND hWnd,
		_In_ BOOL bEnable)
	{
		typedef BOOL(WINAPI *PFN_EnableChildWindowDpiMessage)(HWND, BOOL);
		PFN_EnableChildWindowDpiMessage pEnableChildWindowDpiMessage = (PFN_EnableChildWindowDpiMessage)GetProcAddress(GetModuleHandleW(L"user32.dll"), "EnableChildWindowDpiMessage");
		if (pEnableChildWindowDpiMessage)return pEnableChildWindowDpiMessage(hWnd, bEnable);
		return false;
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

			{ (const uint8_t*) "createURLProtocol",     NULL, &createURLProtocol },
			{ (const uint8_t*) "existURLProtocol",     NULL, &existURLProtocol },
			{ (const uint8_t*) "clearURLProtocol",     NULL, &clearURLProtocol },

			{ (const uint8_t*) "isAdminRun",     NULL, &isAdminRun },

			{ (const uint8_t*) "getProcessHWnds",     NULL, &getProcessHWnds },

			//{ (const uint8_t*) "setProcessDpiAwareness",     NULL, &setProcessDpiAwareness },

			{ (const uint8_t*) "runCoroutine",     NULL, &runCoroutine },

			{ (const uint8_t*) "runExec",     NULL, &runExec },
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

