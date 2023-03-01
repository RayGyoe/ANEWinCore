#pragma once
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>

class StartRun
{

private:

	wchar_t *subKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;
	std::wstring to_wide_string(const std::string& input)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.from_bytes(input);
	}


public:
	bool isStart(std::string appName) 
	{
		std::wstring wsAppName = to_wide_string(appName);

		if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
		{
			//2、得到本程序自身的全路径
			TCHAR strExeFullDir[MAX_PATH];
			GetModuleFileName(NULL, strExeFullDir, MAX_PATH);

			//3、判断注册表项是否已经存在
			TCHAR strDir[MAX_PATH] = {};
			DWORD nLength = MAX_PATH;
			long result = RegGetValue(hKey, nullptr, wsAppName.c_str(), RRF_RT_REG_SZ, 0, strDir, &nLength);

			//4、已经存在
			return result == ERROR_SUCCESS;
		}
		return false;
	}
	bool StartOn(std::string appName) {

		std::wstring wsAppName = to_wide_string(appName);

		if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
		{
			//2、得到本程序自身的全路径
			TCHAR strExeFullDir[MAX_PATH];
			GetModuleFileName(NULL, strExeFullDir, MAX_PATH);

			//3、判断注册表项是否已经存在
			TCHAR strDir[MAX_PATH] = {};
			DWORD nLength = MAX_PATH;
			long result = RegGetValue(hKey, nullptr, wsAppName.c_str(), RRF_RT_REG_SZ, 0, strDir, &nLength);

			//4、已经存在
			if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0) {
				//5、添加一个子Key,并设置值，"GISRestart"是应用程序名字（不加后缀.exe） 
				RegSetValueEx(hKey, wsAppName.c_str(), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));
				//6、关闭注册表
				RegCloseKey(hKey);
				return true;
			}
		}


		return false;
	}

	bool StartOff(std::string appName)
	{
		std::wstring wsAppName = to_wide_string(appName);
		
		if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			//2、删除值
			RegDeleteValue(hKey, wsAppName.c_str());
			//3、关闭注册表
			RegCloseKey(hKey);

			return true;
		}
		return false;
	}

};