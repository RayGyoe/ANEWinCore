#include "D3DStage.h"

// D3DCOLOR is equivalent to D3DFMT_A8R8G8B8
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

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
D3DStage::D3DStage(IDirect3D9 *m_pDirect3D9,int index,HWND hwnd,int x,int y, int width, int height, double scale)
{
	HRESULT hr = S_OK;

	HRESULT lRet;
	InitializeCriticalSection(&m_critial);

	this->width = width;
	this->height = height;
	this->scale = scale;
	this->index = index;

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.style = 0;
	wcex.lpfnWndProc = ChildWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//BLACK_BRUSH
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D3DChildWindowClass"+index;
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);

	//WS_EX_TRANSPARENT
	m_hwndLayeredChild = CreateWindowEx(WS_EX_TRANSPARENT, wcex.lpszClassName, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_EX_LAYERED, x* scale, y* scale, width * scale, height* scale, hwnd, NULL, NULL, NULL);


	printf("\n CreateWindowEx w:%d  h:%d  scale:%f \n", width, height,scale);

	hr = m_hwndLayeredChild ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		printf("\n m_hwndLayeredChild ok  hwnd:%d\n",m_hwndLayeredChild);
		SetLayeredWindowAttributes(m_hwndLayeredChild, 0, 255, LWA_ALPHA);
	}
	else {
		printf("\n m_hwndLayeredChild error\n");
		return;
	}
	//SetWindowPos(m_hwndLayeredChild, HWND_BOTTOM, x, y, width, height, SWP_FRAMECHANGED);
	ShowWindow(m_hwndLayeredChild, SW_SHOWNORMAL);

	///
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = m_hwndLayeredChild;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//D3DFMT_D24S8
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;//D3DFMT_A8R8G8B8  D3DFMT_X8R8G8B8 
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width < 1920 ? 1920 : width;
	d3dpp.BackBufferHeight = height < 1080 ? 1080 : height;
	/*
	if (SUCCEEDED(m_pDirect3D9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, d3dpp.BackBufferFormat, false, D3DMULTISAMPLE_2_SAMPLES, NULL)))
	{
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP	D3DSWAPEFFECT_DISCARD
		d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	}
	else
	{*/
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_FLIP	D3DSWAPEFFECT_DISCARD
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	//}
	

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
	// Turn on the zbuffer

	m_pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	printf("\n MultiSampleType. = %d\n", d3dpp.MultiSampleType);
	if (d3dpp.MultiSampleType != D3DMULTISAMPLE_NONE) {
		m_pDirect3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	}

	this->CreateTextureSurface();
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


void D3DStage::CreateTextureSurface()
{
	//Each 32-bit integer is a combination of four 8 - bit channel values(from 0 to 255) 
	//that describe the alpha transparency and the red, green, and blue(ARGB) values of the pixel. 
	//(For ARGB values, the most significant byte represents the alpha channel value, followed by red, green, and blue.)

	if (m_pDirect3DDevice == NULL)return;
	if (m_pDirect3DSurfaceRender != NULL) return;

	m_pDirect3DDevice->CreateOffscreenPlainSurface(
		this->width, this->height,
		d3dpp.BackBufferFormat,//D3DFMT_A8R8G8B8  D3DFMT_X8R8G8B8 
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);
}

bool D3DStage::Render(uint32_t argc, FREObject argv[], FREContext ctx)
{
	if (m_pDirect3DDevice == NULL)
		return false;

	int32_t _type = 0;
	FREGetObjectAsInt32(argv[1], &_type);

	if (_type == 1) {
		int32_t _width = 0;
		FREGetObjectAsInt32(argv[3], &_width);
		int32_t _height= 0;
		FREGetObjectAsInt32(argv[4], &_height);
		

		if (m_pDirect3DSurfaceRender != NULL && (this->width != _width || this->height != _height) )
		{
			this->width = _width;
			this->height = _height;

			m_pDirect3DSurfaceRender->Release();
			m_pDirect3DSurfaceRender = NULL;

			this->CreateTextureSurface();
		}
	}


	if (m_pDirect3DSurfaceRender == NULL)
		return false;


	HRESULT lRet;
	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
		return false;

	unsigned char *dst = (unsigned char*)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	

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
	HRESULT result = m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST)// && m_pDirect3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET
	{
		printf("\n D3DERR_DEVICELOST \n");
		if (m_pDirect3DSurfaceRender)m_pDirect3DSurfaceRender->Release();
		m_pDirect3DSurfaceRender = NULL;

		std::string level = "d3derror";
		std::string code = std::to_string(this->index) + "||devicelost";
		FREDispatchStatusEventAsync(ctx, reinterpret_cast<const uint8_t *>(code.data()), reinterpret_cast<const uint8_t *>(level.data()));
	}

	return true;
}

bool D3DStage::Visible(bool visible) {
	ShowWindow(m_hwndLayeredChild, visible ? SW_SHOWNORMAL : SW_HIDE);
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
	LeaveCriticalSection(&m_critial);

	m_pDirect3DDevice = NULL;
	m_pDirect3DSurfaceRender = NULL;


	DestroyWindow(m_hwndLayeredChild);

	return true;
}