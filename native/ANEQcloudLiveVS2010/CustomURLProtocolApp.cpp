#include <windows.h>
#include "CustomURLProtocolApp.h"
#include <Strsafe.h>
#include <vector>

#include "ANEWinCore.h"

CustomURLProtocol::CustomURLProtocol()
{
	m_wszProtocolName =	L"CustomProtocol";
	m_wszCompanyName	=	L"CompanyName";
	m_wszAppPath		=	L"Notepad.exe";
}
	
CustomURLProtocol::CustomURLProtocol(std::wstring pwProtocolName, std::wstring pwCompanyName, std::wstring pwAppPath)
{
	m_wszProtocolName =	pwProtocolName;
	m_wszCompanyName	=	pwCompanyName;
	m_wszAppPath		=	pwAppPath;
}

void CustomURLProtocol::FormatErrorMsg()
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					m_dwErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf,
					0, NULL );

	// Display the error message and exit the process
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
	(lstrlen((LPCTSTR)lpMsgBuf)+ 40) * sizeof(TCHAR)); 

	StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("CustomURLProtocol::Failed with error %d: %s"), 
        m_dwErrorCode, lpMsgBuf); 

	m_wszErrorMsg.append((LPCTSTR)lpDisplayBuf);
	OutputDebugStringW((LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

std::string WStringToString(const std::wstring &wstr)
{
	std::string str;
	int nLen = (int)wstr.length();
	str.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
	if (nResult == 0)
	{
		return "";
	}
	return str;
}

std::wstring StringToWString(const std::string& s)
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

int CustomURLProtocol::CreateCustomProtocol()
{
	WCHAR szValue[MAX_PATH] = {0};
	HKEY hKey				= NULL;
	HKEY hKeyDefaultIcon	= NULL;
	HKEY hKeyCommand		= NULL;
	bool IsRegAlreadyPresent=	false;
	
	do
	{
		//HKEY_CURRENT_USER  HKEY_CLASSES_ROOT	HKEY_LOCAL_MACHINE

		std::string protocolName = "Software\\Classes\\" + WStringToString(m_wszProtocolName);
		std::wstring wsProtocolName = StringToWString(protocolName);
		if((m_dwErrorCode = RegOpenKeyExW(HKEY_CURRENT_USER, wsProtocolName.c_str(), 0L,  KEY_READ, &hKey)) != ERROR_SUCCESS)
		{
			
			if((m_dwErrorCode = RegCreateKeyExW(HKEY_CURRENT_USER,
						wsProtocolName.c_str(),
						0,
						NULL,
						REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,
						NULL,
						&hKey,
						NULL)) == ERROR_SUCCESS)
			{
				swprintf_s(szValue,MAX_PATH,URL_PROTOCOL_STRING,m_wszProtocolName.c_str());

				if((m_dwErrorCode = RegSetValueExW( hKey,L"",0,REG_SZ,(BYTE *)&szValue,wcslen(szValue)*2+2)) != ERROR_SUCCESS)
					break;

				if((m_dwErrorCode = RegSetValueExW( hKey,URL_PROTOCOL,0,REG_SZ,(BYTE *)"",sizeof(REG_SZ))) != ERROR_SUCCESS)
					break;

				if( (m_dwErrorCode = RegCreateKeyW(hKey,URL_PROTOCOL_DEFAULTICON,&hKeyDefaultIcon)) == ERROR_SUCCESS )
				{
					swprintf_s(szValue,MAX_PATH,L"%s",m_wszAppPath.c_str());
					if((m_dwErrorCode = RegSetValueExW( hKeyDefaultIcon,L"",0,REG_SZ,(BYTE *)&szValue,wcslen(szValue)*2 + 2)) != ERROR_SUCCESS)
						break;

					if( (m_dwErrorCode = RegCreateKeyW(hKey,URL_PROTOCOL_COMMAND,&hKeyCommand)) == ERROR_SUCCESS )
					{
						swprintf_s(szValue,MAX_PATH,L"\"%s\" \"%%1\"",m_wszAppPath.c_str());
						m_dwErrorCode = RegSetValueExW( hKeyCommand,L"",0,REG_SZ,(BYTE *)&szValue,wcslen(szValue)*2+2);
					}
				}
			}
		}
		else
		{
			m_dwErrorCode	=	-1;
			IsRegAlreadyPresent = true;
			swprintf_s(szValue,MAX_PATH,L"%s Key is already present.",m_wszProtocolName.c_str());
			m_wszErrorMsg=szValue;
		}
	}
	while(FALSE);
	
	if(m_dwErrorCode != ERROR_SUCCESS)
		FormatErrorMsg();

	if(hKeyCommand)
	{
		::RegCloseKey(hKeyCommand);
		hKeyCommand = NULL;
	}
	if(hKeyDefaultIcon)
	{
		::RegCloseKey(hKeyDefaultIcon);
		hKeyDefaultIcon = NULL;
	}
	if(hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}

	return m_dwErrorCode;
}



int CustomURLProtocol::DeleteCustomProtocol()
{
	HKEY hKey = NULL;
	//HKEY_CURRENT_USER  HKEY_CLASSES_ROOT

	std::string protocolName = "Software\\Classes\\" + WStringToString(m_wszProtocolName);
	std::wstring wsProtocolName = StringToWString(protocolName);

	if((m_dwErrorCode = RegOpenKeyExW(HKEY_CURRENT_USER, wsProtocolName.c_str(), 0L,  KEY_ALL_ACCESS, &hKey)) == ERROR_SUCCESS)
	{
		if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_DEFAULTICON) == ERROR_SUCCESS)	
			if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_COMMAND) == ERROR_SUCCESS)	
				if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_OPEN) == ERROR_SUCCESS)	
					if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_SHELL) == ERROR_SUCCESS)
						if( m_dwErrorCode = ::RegCloseKey(hKey) == ERROR_SUCCESS)
							m_dwErrorCode = ::RegDeleteKey(HKEY_CURRENT_USER, wsProtocolName.c_str());
	}
	if(m_dwErrorCode != ERROR_SUCCESS)
		FormatErrorMsg();
	return m_dwErrorCode;
}

bool CustomURLProtocol::existCustomProtocol(int keyType)
{
	HKEY hKey = NULL;
	std::string protocolName = "Software\\Classes\\" + WStringToString(m_wszProtocolName);
	std::wstring wsProtocolName = StringToWString(protocolName);


	HKEY hkeyType = HKEY_CURRENT_USER;
	switch (keyType)
	{
		case 0:
			hkeyType = HKEY_CLASSES_ROOT;
			break;
		case 1:
			hkeyType = HKEY_CURRENT_USER;
			break;
		case 2:
			hkeyType = HKEY_LOCAL_MACHINE;
			break;
		case 3:
			hkeyType = HKEY_USERS;
			break;
	}

	if ((m_dwErrorCode = RegOpenKeyExW(hkeyType, wsProtocolName.c_str(), 0L, KEY_READ, &hKey)) == ERROR_SUCCESS)
	{
		return true;
	}
	return false;
}
