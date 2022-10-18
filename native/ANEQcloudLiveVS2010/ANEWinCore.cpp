#include "ANEWinCore.h"


#include "IEProxy.h"
using namespace ie_proxy;

#include "CustomURLProtocolApp.h"

#include "crash-report.h"

#include "FontLoader.h"

#include <psapi.h>
#pragma comment(lib,"psapi.lib")

std::string intToStdString(int value)
{
	std::stringstream str_stream;
	str_stream << value;
	std::string str = str_stream.str();

	return str;
}

std::wstring s2ws2(const std::string& s)
{
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
BOOL stringToWString(const std::string &str, std::wstring &wstr)
{


	int nLen = (int)str.length();
	wstr.resize(nLen, L' ');
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);
	if (nResult == 0)
	{
		return FALSE;
	}
	return TRUE;
}

std::wstring s2ws(const std::string& str)
{

	//定义一个空的std::wstring
	std::wstring wstr = L"";

	/**
	* CodePage:该参数决定执行转换时使用的编码格式,本机使用编码格式为UTF8,所以使用CP_UTF8.
	*dwFlags:使用0即可
	*lpMultiByteStr:要转换的字符串指针,使用c_str()即可获得
	*cbMultiByte:要转换的字符串的长度,字节为单位,可以使用size()获得
	*lpWideCharStr:指向接收转换后字符串的缓冲区的指针
	*cchWideChar:缓冲区大小,如果为0,则返回所需要的缓冲区大小
	*详见https://docs.microsoft.com/zh-cn/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar?redirectedfrom=MSDN
	*/

	//lpWideCharStr设为NULL,cchWideChar设为0,该步骤用于获取缓冲区大小
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);

	//创建wchar_t数组作为缓冲区,用于接收转换出来的内容,数组长度为len+1的原因是字符串需要以'\0'结尾,所以+1用来存储'\0'
	wchar_t* wchar = new wchar_t[len + 1];

	//缓冲区和所需缓冲区大小都已确定,执行转换
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), wchar, len);

	//使用'\0'结尾
	wchar[len] = '\0';

	//缓冲区拼接到std::wstring
	wstr.append(wchar);

	//切记要清空缓冲区,否则内存泄漏
	delete[]wchar;

	return wstr;
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
FREObject newFREObject(const char *value) {
	FREObject result;
	auto status = FRENewObjectFromUTF8(uint32_t(strlen(value)) + 1, reinterpret_cast<const uint8_t *>(value), &result);
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

	const char *TAG = "ANEWinCore";
	

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
			
			CrashReporter crashReporter;
			(void)crashReporter; //prevents unused warnings

			printf("\n\n%s,%s", TAG, "crashDump EnableAutoDump");
		}
		isCrateCrashDump = true;

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}

	FREObject crashTest(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{

		int *test;
		*test = 0;

		return NULL;
	}

	///

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
		std::string szAppPath = ws2s(UTF82Wide(getFREString(argv[1])));// getFREString(argv[1]);
		std::string szCompanyName = getFREString(argv[2]);

		printf("\n%s,%s = %s  = %s  = %s", TAG, "createURLProtocol", szProtocolName.c_str(), szAppPath.c_str() , szCompanyName.c_str());

		
		m_CustomURLProtocol.setProtocolName(s2ws(szProtocolName));
		m_CustomURLProtocol.setCompanyName(s2ws(szCompanyName));

		//std::wstring path;
		//stringToWString(szAppPath, path);
		//m_CustomURLProtocol.setAppPath(path);
		
		m_CustomURLProtocol.setAppPath(s2ws2(szAppPath));

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



	FREObject addFont(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n addFont = \n");

		std::wstring m_szFontFile = s2ws(getFREString(argv[0]));
		int m_nResults = AddFontResourceEx(
											m_szFontFile.c_str(), // font file name
											FR_PRIVATE,             // font characteristics
											NULL);
		
		FREObject result;
		auto status = FRENewObjectFromBool(m_nResults == 0, &result);
		return result;
	}


	FREObject removeFont(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n removeFont = \n");

		std::wstring m_szFontFile = s2ws(getFREString(argv[0]));
		BOOL m_nResults = RemoveFontResourceEx(
			m_szFontFile.c_str(),  // name of font file
			FR_PRIVATE,            // font characteristics
			NULL            // Reserved.
			);

		FREObject result;
		auto status = FRENewObjectFromBool(m_nResults == 0, &result);
		return result;
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
		
		//SW_SHOWNORMAL 1
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

	
	FREObject memoryCollation(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n memoryCollation = threadId=%i\n", GetCurrentThreadId());

		int maxMemory = getInt32(argv[0]);
		printf("\n maxMemory = %i\n", maxMemory);

		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
		double memoryUsage_M = pmc.WorkingSetSize / (1024.0 *1024.0);

		std::cout <<  "内存使用:" << memoryUsage_M << "M" << std::endl;

		if (memoryUsage_M > maxMemory) {
			EmptyWorkingSet(handle);
			std::cout << "运行内存整理" << std::endl;
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


	//
	FREObject getHostByName(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		
		std::string url = getFREString(argv[0]);
		printf("\n getHostByName = %s\n", url.c_str());

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		struct hostent* hostAddr = gethostbyname(url.c_str());
		if (hostAddr == nullptr) {
			std::cout << "无法找到主机" << std::endl;
			return NULL;
		}

		//IP地址

		char *ipv4 = new char[48];
		sprintf(ipv4, "%d.%d.%d.%d", 
			(hostAddr->h_addr_list[0][0] & 0x00ff),
			(hostAddr->h_addr_list[0][1] & 0x00ff),
			(hostAddr->h_addr_list[0][2] & 0x00ff),
			(hostAddr->h_addr_list[0][3] & 0x00ff));

		WSACleanup();

		printf("ip:%s", ipv4);

		return newFREObject(ipv4);

		for (int i = 0; hostAddr->h_addr_list[i]; i++) {
			//这里使用标准转换运算符reinterpret_cast,强制转换会出现警告
			struct in_addr tmpAddr = *reinterpret_cast<struct in_addr*>(hostAddr->h_addr_list[i]);
			char *ch = inet_ntoa(tmpAddr);
			strcat(ipv4, ch);
		}



		FREObject result;
		auto status = FRENewObjectFromUTF8(uint32_t(strlen(ipv4)) + 1, reinterpret_cast<const uint8_t *>(ipv4), &result);
		return result;

	}


	/*
	*获取window的hwnd
	*/
	FREObject getWindowHwnd(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		int hwnd = 0;

		FRENativeWindow nativeWindow;
		FREObject window = argv[0];
		FREResult ret = FREAcquireNativeWindowHandle(window, &nativeWindow);
		if (ret == FRE_OK) {
			hwnd = (int)nativeWindow;
			FREReleaseNativeWindowHandle(window);
		}
		FREObject result;
		auto status = FRENewObjectFromInt32(hwnd, &result);
		return result;
	}
	

	///
	// Flash Native Extensions stuff	
	void ANEWinCoreContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet) {

		static FRENamedFunction extensionFunctions[] =
		{
			{ (const uint8_t*) "isSupported",     NULL, &isSupported },

			{ (const uint8_t*) "crashDump",     NULL, &crashDump },
			{ (const uint8_t*) "crashTest",     NULL, &crashTest },
			
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

			{ (const uint8_t*) "addFont",     NULL, &addFont },
			{ (const uint8_t*) "removeFont",     NULL, &removeFont },

			{ (const uint8_t*) "runCoroutine",     NULL, &runCoroutine },

			{ (const uint8_t*) "runExec",     NULL, &runExec },

			{ (const uint8_t*) "memoryCollation",     NULL, &memoryCollation },

			{ (const uint8_t*) "getHostByName",     NULL, &getHostByName },

			{ (const uint8_t*) "getWindowHwnd",     NULL, &getWindowHwnd },
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

