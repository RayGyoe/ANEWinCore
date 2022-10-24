#pragma once
#include <stdio.h>
#include <tchar.h>
#include <d3d9.h>
#include<iostream>
using namespace std;



class D3DStage
{
public:
	D3DStage(HWND hwnd, unsigned long lWidth, unsigned long lHeight,std::string url);
	bool Render();
	~D3DStage();

private:
	CRITICAL_SECTION  m_critial;
	IDirect3D9 *m_pDirect3D9 = NULL;
	IDirect3DDevice9 *m_pDirect3DDevice = NULL;
	IDirect3DSurface9 *m_pDirect3DSurfaceRender = NULL;
	RECT m_rtViewport;

	FILE *fp = NULL;
	int bpp = 12;
	int screen_w = 320;
	int screen_h = 180;
	int pixel_w = 320;
	int pixel_h = 180;
	unsigned char buffer[320*180*12 / 8];
	void Cleanup();

};
