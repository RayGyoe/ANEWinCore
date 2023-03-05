#pragma once
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

class WinUtils
{
public:
	std::wstring GetHostName()
	{
		std::wstring host_name;
		DWORD name_len = MAX_COMPUTERNAME_LENGTH + 1;
		host_name.resize(name_len);
		bool result = !!::GetComputerName(&host_name[0], &name_len);
		assert(result);
		host_name.resize(name_len);
		return host_name;
	}

	HMODULE GetModuleHandleFromAddress(void* address)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		DWORD result = ::VirtualQuery(address, &mbi, sizeof(mbi));
		assert(result == sizeof(mbi));
		return static_cast<HMODULE>(mbi.AllocationBase);
	}

	
	std::wstring GetModulePathName(HMODULE module_handle)
	{
		//DCHECK(IsModuleHandleValid(module_handle));
		std::wstring mod_path;
		mod_path.resize(MAX_PATH);
		mod_path.resize(::GetModuleFileNameW(module_handle, &mod_path[0], (DWORD)mod_path.size()));
		return mod_path;
	}

	/*
	*/
	bool OpenResource(const wchar_t *resource,
		const wchar_t *type,
		void *&data,
		unsigned long &size,
		HMODULE module)
	{
		HRSRC resource_handle = ::FindResourceW(module,
			resource,
			type);
		if (resource_handle == NULL)
			return false;
		HGLOBAL data_handle = ::LoadResource(module, resource_handle);
		if (data_handle == NULL)
			return false;
		size = ::SizeofResource(module, resource_handle);
		if (size == 0)
			return false;
		data = ::LockResource(data_handle);
		if (data == NULL)
			return false;
		return true;
	}



	// convert string to wstring
	std::wstring to_wide_string(const std::string& input)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.from_bytes(input);
	}
	// convert wstring to string 
	std::string to_byte_string(const std::wstring& input)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(input);
	}
};