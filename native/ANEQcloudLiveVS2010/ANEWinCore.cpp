#include "ANEWinCore.h"

extern "C" {
	
	const char *TAG = "ANEWinCore:";
	

	bool isCrateCrashDump = false;

	//��ʼ��
	FREObject isSupported(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "isSupport");

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}

	//�������
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


	//ǿ�ƹرճ���
	FREObject killProcess(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "killProcess");
		TerminateProcess(GetCurrentProcess(), 0);

		FREObject result;
		auto status = FRENewObjectFromBool(true, &result);
		return result;
	}


	//�������ߣ�����
	FREObject keepScreenOn(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("\n%s,%s", TAG, "keepScreenOn");

		uint32_t result = 0;
		auto ret = false;
		FREGetObjectAsBool(argv[0], &result);
		if (result > 0) ret = true;

		if (ret) {
			//��ֹ��Ļ����
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, false, 0, 0);
			//��ֹ��Դ��������˯�ߡ�����
			SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
		}
		else {
			//�ָ���Ļ����
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, true, 0, 0);
			//�ָ���Դ����
			SetThreadExecutionState(ES_CONTINUOUS);
		}

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
		};

		*numFunctionsToSet = sizeof(extensionFunctions) / sizeof(FRENamedFunction);
		*functionsToSet = extensionFunctions;
	}

	void ANEWinCoreContextFinalizer(void * extData)
	{
		printf("\n%s,%s", TAG, "ANEWinCoreContextFinalizer��release()");
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

