#ifndef CRASH_REPORTER_H
#define CRASH_REPORTER_H

//Exclude rarely used content from the Windows headers.
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    undef WIN32_LEAN_AND_MEAN
#else
#    include <windows.h>
#endif
#include <tchar.h>
#include <DbgHelp.h>
#include <iostream>

class CrashReporter {
public:
	inline CrashReporter() { Register(); }
	inline ~CrashReporter() { Unregister(); }

	inline static void Register() {
		if (m_lastExceptionFilter != NULL) {
			fprintf(stdout, "CrashReporter: is already registered\n");
			fflush(stdout);
		}
		SetErrorMode(SEM_FAILCRITICALERRORS);
		//ensures UnHandledExceptionFilter is called before App dies.
		m_lastExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);
	}
	inline static void Unregister() {
		SetUnhandledExceptionFilter(m_lastExceptionFilter);
	}

private:
	static LPTOP_LEVEL_EXCEPTION_FILTER m_lastExceptionFilter;
	static LONG WINAPI UnHandledExceptionFilter(_EXCEPTION_POINTERS *);
};


#endif // CRASH_REPORTER_H