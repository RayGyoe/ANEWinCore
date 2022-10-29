#include "D3DStage.h"


D3DStage::D3DStage(HWND hwnd, unsigned long lWidth, unsigned long lHeight, std::string url)
{
	HRESULT lRet;
	InitializeCriticalSection(&m_critial);
	Cleanup();

	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;

	//d3dpp.BackBufferWidth = lWidth;
	//d3dpp.BackBufferHeight = lHeight;
	GetClientRect(hwnd, &m_rtViewport);

	m_rtViewport.bottom = 180;
	m_rtViewport.right = 320;

	//D3DCREATE_HARDWARE_VERTEXPROCESSING
	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDirect3DDevice);

	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		lWidth, lHeight,
		(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	printf("\n yuv file,%s.\n",url.c_str());
	fp = fopen(url.c_str(), "rb+");


	if (fp == NULL) {
		printf("\n Cannot open this file.\n");
	}
	else {
		printf("\n open yuv file.\n");
	}
}


bool D3DStage::Render()
{
	HRESULT lRet;
	//Read Data
	//RGB
	if (fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp) != pixel_w*pixel_h*bpp / 8) {
		// Loop
		fseek(fp, 0, SEEK_SET);
		fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp);
	}

	if (m_pDirect3DSurfaceRender == NULL)
		return false;
	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
		return false;

	byte *pSrc = buffer;
	byte * pDest = (BYTE *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	unsigned long i = 0;

	//Copy Data
	for (i = 0; i < pixel_h; i++) {
		memcpy(pDest + i * stride, pSrc + i * pixel_w, pixel_w);
	}
	for (i = 0; i < pixel_h / 2; i++) {
		memcpy(pDest + stride * pixel_h + i * stride / 2, pSrc + pixel_w * pixel_h + pixel_w * pixel_h / 4 + i * pixel_w / 2, pixel_w / 2);
	}
	for (i = 0; i < pixel_h / 2; i++) {
		memcpy(pDest + stride * pixel_h + stride * pixel_h / 4 + i * stride / 2, pSrc + pixel_w * pixel_h + i * pixel_w / 2, pixel_w / 2);
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(lRet))
		return false;

	if (m_pDirect3DDevice == NULL)
		return false;

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	//开始渲染

	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);

	//结束场景渲染
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(&m_rtViewport, &m_rtViewport, NULL, NULL);
	return true;
}

D3DStage::~D3DStage()
{

}

void D3DStage::Cleanup()
{
	EnterCriticalSection(&m_critial);
	if (m_pDirect3DSurfaceRender)
		m_pDirect3DSurfaceRender->Release();
	if (m_pDirect3DDevice)
		m_pDirect3DDevice->Release();
	if (m_pDirect3D9)
		m_pDirect3D9->Release();
	LeaveCriticalSection(&m_critial);
}