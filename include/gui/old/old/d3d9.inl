
#ifdef __cplusplus
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

typedef struct d3d9_rander_t {
  rander_funcs_t* funcs;
  HDC hDC;
  HGLRC hGLRC;
  HWND hWnd;
  int AntiAlias;
  ECOLOR_FORMAT ColorFormat;
  
  LPDIRECT3D9 pD3D;
  LPDIRECT3DDEVICE9 pD3DDevice ;
  D3DPRESENT_PARAMETERS pD3DPresent;
  
} d3d9_rander_t;

void d3d9_Close(d3d9_rander_t* s)
{
}
void d3d9_Clear(d3d9_rander_t* s, int Count, const IRECT* pRects, DWORD Flags, COLOR color)
{
  DWORD Flags1 = 0;
  if (Flags & EDCLEAR_TARGET) {
    Flags1 |= D3DCLEAR_TARGET;
  }
  if (Flags & EDCLEAR_ZBUFFER) {
    Flags1 |= D3DCLEAR_ZBUFFER;
  }
  if (Flags & EDCLEAR_STENCIL) {
    Flags1 |= D3DCLEAR_STENCIL;
  }
  // `�رչ��յļ���`
  s->pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
  // `�ѻ�����Ϊ��ɫ`
  s->pD3DDevice->Clear(
    Count, (D3DRECT*)pRects, // `�����������` 
    Flags1, // `�����ɫ��Z Buffer` 
    color, // `����Ҫ����ɫ��ɺ�ɫ`
    1.0f, // `����Ҫ��Zֵ��Ϊ1, Ҳ�����뾵ͷ��Զ.`
    0 // `����Ҫ��Stencil buffer��Ϊ0, �����Ƿ�����û������.`
	);
}
void d3d9_SetRenderState(d3d9_rander_t* s, int type, int b) {
  switch (type) {
  case EMF_LIGHTING:
    s->pD3DDevice->SetRenderState(D3DRS_LIGHTING, b);
  break;
  default:
    ASSERT(0);
    break;
  }
}
void d3d9_BeginScene(d3d9_rander_t* s)
{
  s->pD3DDevice->BeginScene();
}
void d3d9_EndScene(d3d9_rander_t* s)
{
  s->pD3DDevice->EndScene();
	s->pD3DDevice->Present( NULL, NULL, NULL, NULL );
}
void d3d9_DrawPrimitiveUP(d3d9_rander_t* s, E_PRIMITIVE_TYPE t, int count, const void* vertex, int stride) {
  D3DPRIMITIVETYPE PrimitiveType = D3DPT_FORCE_DWORD;
  switch (t) {
#define CASEDEF(a, b)  case a: PrimitiveType = b; break
    CASEDEF(EPT_POINTS, D3DPT_POINTLIST);
    CASEDEF(EPT_LINES, D3DPT_LINELIST);
    CASEDEF(EPT_LINE_STRIP, D3DPT_LINESTRIP);
#undef CASEDEF
  };
	s->pD3DDevice->SetFVF(D3DFVF_XYZ); 
	s->pD3DDevice->DrawPrimitiveUP(PrimitiveType, count, vertex, stride); 
}
d3d9_rander_t* d3d9_init(void* WindowId, rander_params_t* Params) {
  d3d9_rander_t s[1] = {0};
  static rander_funcs_t d3d9_funcs[1] = {0};
  static int inited=0;
  int h, w;
  
	// `�򿪴���ʱ���ѻ��������������ڵ�ָ��, ֱ������ʹ��.`
	HWND hWnd = (HWND)WindowId;

	IRECT rect;

  
  if (!inited) {
    RANDER_FUNCS_SET(d3d9_funcs, d3d9);
    d3d9_funcs->dt = EDT_DIRECT3D9;
    inited = 1;
  }
  s->funcs = d3d9_funcs;
  s->hWnd = (HWND)WindowId;
  s->hDC = GetDC(s->hWnd);

	GetWindowRect(hWnd, (RECT*)&rect);
	
	w = rect.r - rect.l;
	h = rect.b - rect.t;
	if ( w==0 || h==0 )
		return false;

	// `���һ��D3D9����, ����Ψһ������ȥ����������������ͼ��D3D9 Device.`
	if( NULL == ( s->pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	ZeroMemory( &s->pD3DPresent, sizeof(s->pD3DPresent) );
	s->pD3DPresent.Windowed = TRUE;
	s->pD3DPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
	s->pD3DPresent.BackBufferFormat = D3DFMT_UNKNOWN;
	s->pD3DPresent.BackBufferCount = 1; // `�ṩһ��backbuffer`
	s->pD3DPresent.EnableAutoDepthStencil = TRUE;
	s->pD3DPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
	s->pD3DPresent.MultiSampleType = (D3DMULTISAMPLE_TYPE) Params->MultiSamples;
	s->pD3DPresent.hDeviceWindow = hWnd;

	bool device_initialized = false;

	//`������4�ֲ�ͬ��������d3d9`
	//1. D3DDEVTYPE_HAL + D3DCREATE_HARDWARE_VERTEXPROCESSING 		`��ȫ����GPU����`
	//2. D3DDEVTYPE_HAL + D3DCREATE_MIXED_VERTEXPROCESSING			  `Vertex����CPU��GPU������, ������GPU.`
	//3. D3DDEVTYPE_HAL + D3DCREATE_SOFTWARE_VERTEXPROCESSING 		`Vertex��CPU, ������GPU.`
	//4. D3DDEVTYPE_REF + D3DCREATE_SOFTWARE_VERTEXPROCESSING			`��ȫ��CPU����, ���Ǹ��ǳ�����ģʽ.`

	const int device_types = 4;

	struct sDeviceType
	{
		D3DDEVTYPE type;
		DWORD behavior;
	};

	sDeviceType device_type[device_types] = 
	{
		{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_MIXED_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING}
	};

	for ( int type=0; type<device_types; type++ ) {
		// `����ȥ��һ��Direct3D9�豸`
		if( s->pD3D->CreateDevice( D3DADAPTER_DEFAULT, device_type[type].type, hWnd,
							  device_type[type].behavior, &s->pD3DPresent, &s->pD3DDevice )==D3D_OK )
		{
			device_initialized = true;
			break;
		}
	}

  return (d3d9_rander_t*)memdup(s, sizeof(*s));
}

#endif // __cplusplus
