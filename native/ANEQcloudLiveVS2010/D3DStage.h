#pragma once
#include <stdio.h>
#include <tchar.h>
#include <d3d9.h>
#include<iostream>

#include <assert.h>
#include <inttypes.h>
#include <tmmintrin.h>
#include <string>
using namespace std;

#include "FlashRuntimeExtensions.h"





#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class D3DStage
{
public:

	D3DStage(int index, HWND hwnd, int x, int y, int width, int height, double scale);
	bool Render(uint32_t argc, FREObject argv[], FREContext ctx);
	bool Visible(bool visible);
	bool Resize(int x, int y, int w, int h);

	bool Destroy();
private:
	D3DPRESENT_PARAMETERS d3dpp;

	CRITICAL_SECTION  m_critial;
	IDirect3D9 *m_pDirect3D9 = NULL;
	IDirect3DDevice9 *m_pDirect3DDevice = NULL;
	IDirect3DSurface9 *m_pDirect3DSurfaceRender = NULL;
	//RECT m_rtViewport;

	int index;
	void CreateTextureSurface();
	HWND m_hwndLayeredChild;
	int width;
	int height;
	double scale;
};
