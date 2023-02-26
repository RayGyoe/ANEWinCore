#pragma once
#include <stdio.h>
#include <tchar.h>
#include <d3d9.h>
#include<iostream>

using namespace std;

#include "FlashRuntimeExtensions.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class D3DStage
{
public:
	D3DStage(HWND hwnd, unsigned long lWidth, unsigned long lHeight,std::string url);
	bool Render(uint32_t argc, FREObject argv[]);

	void Resize(int x, int y, int w, int h);

	~D3DStage();

private:
	CRITICAL_SECTION  m_critial;
	IDirect3D9 *m_pDirect3D9 = NULL;
	IDirect3DDevice9 *m_pDirect3DDevice = NULL;
	IDirect3DSurface9 *m_pDirect3DSurfaceRender = NULL;
	//RECT m_rtViewport;

	HWND m_hwndLayeredChild;

	FILE *fp = NULL;
	int bpp = 12;

	int pixel_w = 320;
	int pixel_h = 180;
	unsigned char buffer[320 * 180 * 12 / 8];
	void Cleanup();

};
