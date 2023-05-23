#include "ANEWinCore.h"

#include "win_utils.h"

#include "IEProxy.h"
using namespace ie_proxy;

#include "CustomURLProtocolApp.h"

#include "crash-report.h"

#include "FontLoader.h"


#include <psapi.h>
#pragma comment(lib,"psapi.lib")

#include "D3DStage.h"
#include "StartRun.h"
#include "win_utils.h"
#include "MP4File.h"


#define WM_MY_MESSAGE      WM_USER + 1036
//===================================================================
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
std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然會出現尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
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

char*  wcharToChar(const wchar_t* wch)
{
	char* _char;
	int len = WideCharToMultiByte(CP_ACP, 0, wch, wcslen(wch), NULL, 0, NULL, NULL);
	_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wch, wcslen(wch), _char, len, NULL, NULL);
	_char[len] = '\0';
	return _char;
}






extern "C" {

	const char *TAG = "ANEWinCore";
	

	using fnRtlGetNtVersionNumbers = void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);

	using fnShouldSystemUseDarkMode = bool (WINAPI *)(); // ordinal 138
	using fnIsDarkModeAllowedForApp = bool (WINAPI *)(); // ordinal 139


	DWORD g_buildNumber = 0;
	constexpr bool CheckBuildNumber(DWORD buildNumber)
	{
		return (buildNumber == 17763 || // 1809
			buildNumber == 18362 || // 1903
			buildNumber == 18363 || // 1909
			buildNumber >= 19041); // 2004
	}

	fnShouldSystemUseDarkMode _ShouldSystemUseDarkMode = nullptr;


	bool _isDarkMode = false;


	bool isCrateCrashDump = false;


	IDirect3D9 *m_pDirect3D9 = NULL;
	D3DStage *d3dpp;
	std::map<int, D3DStage*>VectorD3dStage;
	int d3dStage_Index = 0;

	CustomURLProtocol m_CustomURLProtocol;

	WinUtils winUtils;
	StartRun m_StartRun;



	//
	int VIDEO_WIDTH = 1920;
	int VIDEO_HEIGHT = 1080;
	std::map<int, MP4File*>VectorMp4Record;
	int mp4Record_Index = 0;

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
	FREObject memoryTest(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		int* Test = new int[1024 * 1024 * 100];
		printf("\n%s,%s", TAG, "memoryTest");
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
		std::wstring m_szFontFile = s2ws(getFREString(argv[0]));

		printf("\n addFont = %ls \n",m_szFontFile);

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

		std::wstring m_szFontFile = s2ws(getFREString(argv[0]));
		printf("\n removeFont = %ls \n", m_szFontFile);
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

	FREObject postMessage(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		HWND hWnd = (HWND)getInt32(argv[0]);
		std::string message = getFREString(argv[1]);
		::PostMessage(hWnd,WM_MY_MESSAGE,(WPARAM)s2ws(message).c_str(), (LPARAM)s2ws(message).c_str());
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

		printf("\tPageFaultCount: %d\n", pmc.PageFaultCount / 1024 / 1024);
		printf("\tPeakWorkingSetSize: %d\n",pmc.PeakWorkingSetSize / 1024 / 1024);
		printf("\tWorkingSetSize: %d\n", pmc.WorkingSetSize / 1024 / 1024);
		printf("\tQuotaPeakPagedPoolUsage: %d\n",pmc.QuotaPeakPagedPoolUsage / 1024 / 1024);
		printf("\tQuotaPagedPoolUsage: %d\n",pmc.QuotaPagedPoolUsage / 1024 / 1024);
		printf("\tQuotaPeakNonPagedPoolUsage: %d\n",pmc.QuotaPeakNonPagedPoolUsage / 1024 / 1024);
		printf("\tQuotaNonPagedPoolUsage: %d\n",pmc.QuotaNonPagedPoolUsage / 1024 / 1024);
		printf("\tPagefileUsage: %d\n", pmc.PagefileUsage / 1024 / 1024);
		printf("\tPeakPagefileUsage: %d\n",	pmc.PeakPagefileUsage/1024 / 1024);

		/*
		PageFaultCount	页错误数。
		PeakWorkingSetSize	峰值工作集大小（以字节为单位）。
		WorkingSetSize	当前工作集大小（以字节为单位）。
		QuotaPeakPagedPoolUsage	峰值分页池使用情况（以字节为单位）。
		QuotaPagedPoolUsage	当前页池使用情况（以字节为单位）。
		QuotaPeakNonPagedPoolUsage	峰值非分页池使用情况（以字节为单位）。
		QuotaNonPagedPoolUsage	当前非分页池使用情况（以字节为单位）。
		PagefileUsage	虚拟内存总量（以字节为单位）。
		PeakPagefileUsage	峰值虚拟虚拟内存总量（以字节为单位）。
		*/


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



	FREObject isDarkMode(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		if (!_isDarkMode) {
			auto RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
			if (RtlGetNtVersionNumbers)
			{
				DWORD major, minor;
				RtlGetNtVersionNumbers(&major, &minor, &g_buildNumber);
				g_buildNumber &= ~0xF0000000;

				printf("\n major %d,%d\n", major, g_buildNumber);

				if (major == 10 && minor == 0 && CheckBuildNumber(g_buildNumber))
				{
					HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, 0x00000800);
					if (hUxtheme)
					{
						_ShouldSystemUseDarkMode = reinterpret_cast<fnShouldSystemUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(138)));
						if (_ShouldSystemUseDarkMode && _ShouldSystemUseDarkMode())
						{
							_isDarkMode = true;
						}
					}
				}
			}
		}		
		FREObject result;
		auto status = FRENewObjectFromBool(_isDarkMode, &result);
		return result;
	}

	FREObject d3dInit(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		int  index_d3d = -1;


		if (m_pDirect3D9 == NULL) {
			m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		}
		if (m_pDirect3D9 == NULL) {
			FREObject result;
			auto status = FRENewObjectFromInt32(index_d3d, &result);
			return result;
		}


		FRENativeWindow nativeWindow;
		FREObject window = argv[0];
		FREResult ret = FREAcquireNativeWindowHandle(window, &nativeWindow);
		if (ret == FRE_OK) {
			FREReleaseNativeWindowHandle(window);

			//std::string url = getFREString(argv[1]);
			//d3dpp = new D3DStage((HWND)nativeWindow, 320, 180, url);

			int x = getInt32(argv[1]);
			int y = getInt32(argv[2]);
			int width = getInt32(argv[3]);
			int height = getInt32(argv[4]);
			double scale;
			FREGetObjectAsDouble(argv[5],&scale);

			int index = d3dStage_Index += 1;
			D3DStage *stage = new D3DStage(m_pDirect3D9,index, (HWND)nativeWindow,x,y,width,height, scale);
			VectorD3dStage[index] = stage;
			printf("\n Index %d\n", index);
			index_d3d = index;
			//遍历窗口中的子窗口
			//EnumChildWindows((HWND)nativeWindow, EnumChildProc,NULL);
			//HWND child1 = GetWindow((HWND)nativeWindow, GW_CHILD);
			//HWND child2 = GetWindow((HWND)nativeWindow, GW_HWNDNEXT);
			//printf("\n GetWindow child1= %d  child2=%d \n", child1, child2);
			//SetWindowPos(child2, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		}

		FREObject result;
		auto status = FRENewObjectFromInt32(index_d3d, &result);
		return result;
	}



	FREObject d3dRender(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;
		int index = getInt32(argv[0]);

		D3DStage *stage = VectorD3dStage[index];
		if (stage) {
			ret = stage->Render(argc, argv,ctx);
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;

		/*
		bool ret = false;
		if (d3dpp != nullptr)
		{
			ret = d3dpp->Render(argc, argv);
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
		*/
	}

	FREObject d3dVisible(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;
		int index = getInt32(argv[0]);

		D3DStage *stage = VectorD3dStage[index];
		if (stage) {
			uint32_t visible;
			FREGetObjectAsBool(argv[1], &visible);
			ret = stage->Visible((bool)visible);
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}
	FREObject d3dResize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;
		int index = getInt32(argv[0]);


		D3DStage *stage = VectorD3dStage[index];
		if (stage) {
			int x = getInt32(argv[1]);
			int y = getInt32(argv[2]);
			int w = getInt32(argv[3]);
			int h = getInt32(argv[4]);
			ret = stage->Resize(x, y, w, h);
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}



	FREObject d3dDestroy(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;
		int index = getInt32(argv[0]);


		D3DStage *stage = VectorD3dStage[index];
		if (stage) {
			printf("\n Destroy Index %d\n", index);
			ret = stage->Destroy();
			
			VectorD3dStage[index] = NULL;
		}
		
		bool destroyD3d = true;
		for (auto &kv : VectorD3dStage)
		{
			D3DStage *stage = kv.second;
			if (stage) {
				destroyD3d = false;
				break;
			}
		}
		if (destroyD3d) {
			VectorD3dStage.clear();
			printf("\n Destroy Direct3DCreate9 \n");

			if (m_pDirect3D9)m_pDirect3D9->Release();
			m_pDirect3D9 = NULL;
		}

		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}


	//是否启用了开机启动
	FREObject isAutoStart(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{

		std::string appName = getFREString(argv[0]);

		bool ret = m_StartRun.isStart(appName);

		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}

	FREObject updateAutoStart(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;

		////
		std::string appName = getFREString(argv[0]);
		uint32_t isRun;
		FREGetObjectAsBool(argv[1],&isRun);
		
		if (isRun) {
			ret = m_StartRun.StartOn(appName);
		}
		else {
			ret = m_StartRun.StartOff(appName);
		}

		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}

	
	/*
	允许拖拽文件到窗口
	*/
	FREObject dragAcceptFiles(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;

		uint32_t value;
		FREGetObjectAsBool(argv[1], &value);
		////
		FRENativeWindow nativeWindow;
		FREObject window = argv[0];
		if (FREAcquireNativeWindowHandle(window, &nativeWindow) == FRE_OK)
		{
			DragAcceptFiles((HWND)nativeWindow, value==1?TRUE : FALSE);
			printf("\nDragAcceptFiles,%d  = %d\n", nativeWindow, value);
			FREReleaseNativeWindowHandle(window);
			ret = true;
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}


	/*
	获取计算机名称
	*/
	FREObject getHostName(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		////
		std::string hostName = string_To_UTF8(wcharToChar(winUtils.GetHostName().c_str()));
		return newFREObject(hostName.c_str());
	}
	


	HBITMAP GetBitmap()
	{
		HDC hScreen = GetDC(NULL);
		HDC hDC = CreateCompatibleDC(hScreen);

		HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, VIDEO_WIDTH, VIDEO_HEIGHT);
		HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
		BOOL bRet = BitBlt(hDC, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, hScreen, 0, 0, SRCCOPY);

		SelectObject(hDC, old_obj);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);

		return hBitmap;
	}


	
	FREObject initRecordMp4(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		////
		int index = -1;
		///
		std::string file = getFREString(argv[0]);
		int fps = getInt32(argv[1]);
		int width = getInt32(argv[2]);
		int height = getInt32(argv[3]);


		std::wstring filePath = winUtils.to_wide_string(file);
		///
		MP4File *mp4Record = new MP4File(filePath, fps, width, height); // Do not specify file extension. You will get an mp4 file regardless

		mp4Record_Index += 1;
		index = mp4Record_Index;
		VectorMp4Record[index] = mp4Record;

		FREObject result;
		auto status = FRENewObjectFromInt32(index, &result);
		return result;
	}

	FREObject AppendFrameRecordMp4(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;
		HRESULT hr = NULL;

		int index = getInt32(argv[0]);
		MP4File *mp4Record = VectorMp4Record[index];
		if (mp4Record) {
			//printf("\n RecordMp4AppendFrame %d\n", index);
			//HBITMAP hBitmap = GetBitmap();
			//hr = mp4Record->AppendFrame(hbmp);

			int width = getInt32(argv[1]);
			int height = getInt32(argv[2]);
			FREObject objectByteArray = argv[3];
			FREByteArray byteArray;
			FREAcquireByteArray(objectByteArray, &byteArray);
			HBITMAP hBm = CreateBitmap(width, height, 1, 32, byteArray.bytes); // 1 plane, 32 bits
			FREReleaseByteArray(objectByteArray);
			
			hr = mp4Record->AppendFrame(hBm);
			if (!FAILED(hr))
			{
				ret = true;
			}
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
		return result;
	}
	FREObject FinalizeRecordMp4(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool ret = false;

		int index = getInt32(argv[0]);
		MP4File *mp4Record = VectorMp4Record[index];
		if (mp4Record) {
			mp4Record->Finalize();
			ret = true;
		}
		FREObject result;
		auto status = FRENewObjectFromBool(ret, &result);
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
			{ (const uint8_t*) "memoryTest",     NULL, &memoryTest },
			
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
			{ (const uint8_t*) "postMessage",     NULL, &postMessage },


			{ (const uint8_t*) "memoryCollation",     NULL, &memoryCollation },

			{ (const uint8_t*) "getHostByName",     NULL, &getHostByName },

			{ (const uint8_t*) "getWindowHwnd",     NULL, &getWindowHwnd },
			{ (const uint8_t*) "isDarkMode",     NULL, &isDarkMode },
			
			{ (const uint8_t*) "d3dInit",     NULL, &d3dInit },
			{ (const uint8_t*) "d3dRender",     NULL, &d3dRender },
			{ (const uint8_t*) "d3dResize",     NULL, &d3dResize },
			{ (const uint8_t*) "d3dVisible",     NULL, &d3dVisible },
			{ (const uint8_t*) "d3dDestroy",     NULL, &d3dDestroy },


			{ (const uint8_t*) "isAutoStart",     NULL, &isAutoStart },
			{ (const uint8_t*) "updateAutoStart",     NULL, &updateAutoStart },

			{ (const uint8_t*) "dragAcceptFiles",     NULL, &dragAcceptFiles },

			{ (const uint8_t*) "getHostName",     NULL, &getHostName },
			

			//录制mp4
			{ (const uint8_t*) "initRecordMp4",     NULL, &initRecordMp4 },
			{ (const uint8_t*) "AppendFrameRecordMp4",     NULL, &AppendFrameRecordMp4 },
			{ (const uint8_t*) "FinalizeRecordMp4",     NULL, &FinalizeRecordMp4 },
			
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