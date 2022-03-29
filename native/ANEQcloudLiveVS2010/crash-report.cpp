#include "crash-report.h"

#include <stdio.h>

LPTOP_LEVEL_EXCEPTION_FILTER CrashReporter::m_lastExceptionFilter = NULL;

typedef BOOL(WINAPI *MiniDumpWriteDumpFunc)(HANDLE hProcess, DWORD ProcessId
	, HANDLE hFile
	, MINIDUMP_TYPE DumpType
	, const MINIDUMP_EXCEPTION_INFORMATION *ExceptionInfo
	, const MINIDUMP_USER_STREAM_INFORMATION *UserStreamInfo
	, const MINIDUMP_CALLBACK_INFORMATION *Callback
	);

LONG WINAPI CrashReporter::UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionPtr)
{
	//we load DbgHelp.dll dynamically, to support Windows 2000
	HMODULE hModule = ::LoadLibraryA("DbgHelp.dll");
	if (hModule) {
		MiniDumpWriteDumpFunc dumpFunc = reinterpret_cast<MiniDumpWriteDumpFunc>(
			::GetProcAddress(hModule, "MiniDumpWriteDump")
			);
		if (dumpFunc) {
			//fetch system time for dump-file name
			SYSTEMTIME  SystemTime;
			::GetLocalTime(&SystemTime);
			//choose proper path for dump-file
			wchar_t dumpFilePath[MAX_PATH] = { 0 };
			_snwprintf_s(dumpFilePath, MAX_PATH, L"crash_%04d-%d-%02d_%d-%02d-%02d.dmp"
				, SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay
				, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond
			);

			//create and open the dump-file
			HANDLE hFile = ::CreateFileW(dumpFilePath, GENERIC_WRITE
				, FILE_SHARE_WRITE
				, NULL
				, CREATE_ALWAYS
				, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN
				, NULL
			);

			if (hFile != INVALID_HANDLE_VALUE) {
				_MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
				exceptionInfo.ThreadId = GetCurrentThreadId();
				exceptionInfo.ExceptionPointers = exceptionPtr;
				exceptionInfo.ClientPointers = NULL;
				//at last write crash-dump to file
				bool ok = dumpFunc(::GetCurrentProcess(), ::GetCurrentProcessId()
					, hFile, MiniDumpNormal
					, &exceptionInfo, NULL, NULL
				);
				//dump-data is written, and we can close the file
				CloseHandle(hFile);
				if (ok) {
					//Return from UnhandledExceptionFilter and execute the associated exception handler.
					//  This usually results in process termination.
					return EXCEPTION_EXECUTE_HANDLER;
				}
			}
		}
	}
	else {
		std::cout << "LoadLibrary Debug Error!" << std::endl;
	}
	//Proceed with normal execution of UnhandledExceptionFilter.
	//  That means obeying the SetErrorMode flags,
	//  or invoking the Application Error pop-up message box.
	return EXCEPTION_CONTINUE_SEARCH;
}