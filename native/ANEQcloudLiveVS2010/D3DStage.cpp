#include "D3DStage.h"

// D3DCOLOR is equivalent to D3DFMT_A8R8G8B8
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

// maps unsigned 8 bits/channel to D3DCOLOR
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)


typedef unsigned char		uint8;



LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{

	if (message == WM_CREATE)
	{
		// 设置分层属性
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
D3DStage::D3DStage(int index,HWND hwnd,int x,int y, int width, int height, double scale)
{
	HRESULT hr = S_OK;

	HRESULT lRet;
	InitializeCriticalSection(&m_critial);

	this->width = width;
	this->height = height;
	this->scale = scale;

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.style = 0;
	wcex.lpfnWndProc = ChildWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D3DChildWindow"+index;
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);
	m_hwndLayeredChild = CreateWindowEx(0, wcex.lpszClassName, NULL, WS_CHILD | WS_CLIPSIBLINGS, x* scale, y* scale, width * scale, height* scale, hwnd, NULL, NULL, NULL);


	printf("\n CreateWindowEx w:%d  h:%d  scale:%f \n", width, height,scale);

	hr = m_hwndLayeredChild ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		printf("\n m_hwndLayeredChild ok\n");
		if (!SetLayeredWindowAttributes(m_hwndLayeredChild, 0, 255, LWA_ALPHA))// LWA_ALPHA
		{
			printf("\n SetLayeredWindowAttributes error\n");
			return;
		}
	}

	//SetWindowPos(m_hwndLayeredChild, HWND_BOTTOM, x, y, width, height, SWP_FRAMECHANGED);
	ShowWindow(m_hwndLayeredChild, SW_SHOWNORMAL);



	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)return;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = m_hwndLayeredChild;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//D3DFMT_D24S8
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_FLIP	D3DSWAPEFFECT_DISCARD
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	//d3dpp.BackBufferWidth = width;
	//d3dpp.BackBufferHeight = height;
	

	//
	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hwndLayeredChild,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,//D3DCREATE_SOFTWARE_VERTEXPROCESSING	D3DCREATE_HARDWARE_VERTEXPROCESSING
		&d3dpp,
		&m_pDirect3DDevice);


	if (m_pDirect3DDevice == NULL) {

		printf("\n not m_pDirect3DDevice. = %d\n", lRet);
		return;
	}

	//Each 32-bit integer is a combination of four 8 - bit channel values(from 0 to 255) 
	//that describe the alpha transparency and the red, green, and blue(ARGB) values of the pixel. 
	//(For ARGB values, the most significant byte represents the alpha channel value, followed by red, green, and blue.)
	
	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		width, height,
		d3dpp.BackBufferFormat,//D3DFMT_A8R8G8B8  D3DFMT_X8R8G8B8 
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	if (m_pDirect3DSurfaceRender == NULL)
	{
		printf("\n not m_pDirect3DSurfaceRender. = %d\n", lRet);
		return;
	}
}

//argb -> bgr
void convert(uint8_t *orig, size_t imagesize, uint8_t *dest) {
	assert((uintptr_t)orig % 16 == 0);
	assert(imagesize % 4 == 0);
	__m128i mask = _mm_set_epi8(-128, -128, -128, -128, 13, 14, 15, 9, 10, 11, 5, 6, 7, 1, 2, 3);
	uint8_t *end = orig + imagesize * 4;
	for (; orig != end; orig += 16, dest += 12) {
		_mm_storeu_si128((__m128i *)dest, _mm_shuffle_epi8(_mm_load_si128((__m128i *)orig), mask));
	}
}

bool D3DStage::Render(uint32_t argc, FREObject argv[])
{
	if (m_pDirect3DDevice == NULL)
		return false;


	if (m_pDirect3DSurfaceRender == NULL)
		return false;

	HRESULT lRet;
	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
		return false;

	unsigned char *dst = (unsigned char*)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	
	int32_t _type = 0;
	FREGetObjectAsInt32(argv[1], &_type);

	//copy bytearray
	if (_type == 1) {
		FREObject objectByteArray = argv[2];
		FREByteArray byteArray;
		FREAcquireByteArray(objectByteArray, &byteArray);
		memcpy(dst, byteArray.bytes, byteArray.length);
		/*
		for (unsigned long i = 0; i< this->height; i++) {
			memcpy(dst, byteArray.bytes, this->width);
			dst += stride;
			byteArray.bytes += this->width;
		}*/

		FREReleaseByteArray(objectByteArray);
		//printf("\n stride=%d,\n", stride);
	}
	else if (_type == 2) {
		FREObject bitmap_data_object = argv[2];
		FREBitmapData bitmapData;
		FREAcquireBitmapData(bitmap_data_object, &bitmapData);
		//memcpy(d3d_rect.pBits, bitmapData.bits32, bitmapData.width * bitmapData.height * 4);

		int buflength = bitmapData.width * bitmapData.height * 4;

		//memcpy(dst, bitmapData.bits32, buflength);
		for (unsigned long i = 0; i< this->height; i++) {
			memcpy(dst, bitmapData.bits32, this->width);
			dst += stride;
			bitmapData.bits32 += this->width;
		}

		//convert((uint8_t*)bitmapData.bits32, sizeof bitmapData.bits32, dst);

		//d3d_rect.Pitch = bitmapData.lineStride32;
	//	printf("\n  Pitch=%d isPremultiplied=%d  lineStride32=%d  \n", stride, bitmapData.isPremultiplied, bitmapData.lineStride32);
		FREReleaseBitmapData(bitmap_data_object);
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(lRet))
		return false;

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	//开始渲染

	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);

	//结束场景渲染
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	return true;

	return true;
}




bool D3DStage::Resize(int x, int y, int w, int h)
{
	printf("\n Resize %d,%d,%d,%d\n", x, y, w, h);
	SetWindowPos(m_hwndLayeredChild, HWND_BOTTOM, x, y, w, h, SWP_FRAMECHANGED);//SWP_NOMOVE | SWP_NOSIZE |
	//D3DVIEWPORT9 viewData = { 0, 0, w, h, 0.0f, 1.0f };
	//m_pDirect3DDevice->SetViewport(&viewData);

	return true;
}

bool D3DStage::Destroy() {
	EnterCriticalSection(&m_critial);
	if (m_pDirect3DSurfaceRender)
		m_pDirect3DSurfaceRender->Release();
	if (m_pDirect3DDevice)
		m_pDirect3DDevice->Release();
	if (m_pDirect3D9)
		m_pDirect3D9->Release();
	LeaveCriticalSection(&m_critial);

	m_pDirect3DDevice = NULL;
	m_pDirect3DSurfaceRender = NULL;
	return true;
}



D3DStage::D3DStage(HWND hwnd, unsigned long lWidth, unsigned long lHeight, std::string url)
{
	HRESULT hr = S_OK;

	HRESULT lRet;
	InitializeCriticalSection(&m_critial);
	Cleanup();



	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.style = 0;
	wcex.lpfnWndProc = ChildWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D3DChildWindow";
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);
	m_hwndLayeredChild = CreateWindowEx(0,L"D3DChildWindow",NULL,WS_CHILD | WS_CLIPSIBLINGS,0,0, lWidth, lHeight,hwnd,NULL, NULL,NULL);


	printf("\n CreateWindowEx w:%d  h:%d \n", lWidth, lHeight);

	hr = m_hwndLayeredChild ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		printf("\n m_hwndLayeredChild ok\n");
		if (!SetLayeredWindowAttributes(m_hwndLayeredChild, 0, 255, LWA_ALPHA))// LWA_ALPHA
		{
			printf("\n SetLayeredWindowAttributes error\n");
			return;
		}
	}
	
	ShowWindow(m_hwndLayeredChild, SW_SHOWNORMAL);
	//SetWindowPos(m_hwndLayeredChild, HWND_BOTTOM, 0, 0, 0, 0,  SWP_FRAMECHANGED);
	//UpdateWindow(m_hwndLayeredChild);


	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = m_hwndLayeredChild;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.BackBufferWidth = lWidth;
	d3dpp.BackBufferHeight = lHeight;

	//GetClientRect(m_hwndLayeredChild, &m_rtViewport);

	//
	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hwndLayeredChild,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,//D3DCREATE_SOFTWARE_VERTEXPROCESSING	D3DCREATE_HARDWARE_VERTEXPROCESSING
		&d3dpp,
		&m_pDirect3DDevice);


	if (m_pDirect3DDevice == NULL) {

		printf("\n not m_pDirect3DDevice. = %d\n", lRet);
		return;
	}

	
	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		lWidth, lHeight,
		(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),//D3DFMT_A8R8G8B8,//
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	if (m_pDirect3DSurfaceRender == NULL)
	{
		printf("\n not m_pDirect3DSurfaceRender. = %d\n", lRet);
		return;
	}


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
	if (m_pDirect3DDevice == NULL)
		return false;


	if (fp == NULL) {
		return false;
	}

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


	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	//开始渲染

	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);

	//结束场景渲染
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
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