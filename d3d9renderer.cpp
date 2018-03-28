#include <d3d9.h>
#include "pch.h"
#include "d3d9renderer.h"
#pragma comment (lib, "d3d9.lib")

#include "ObjLoader.h"
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

struct MESH_VERTEX
{
	D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal; // The position
	D3DCOLOR color;    // The color
	FLOAT u, v;   // The texture coordinates
};

#define D3DFVF_MESH_VERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct MESH_WIRE_VERTEX
{
	D3DXVECTOR3 position; // The position
	D3DCOLOR color;    // The color
};

#define D3DFVF_MESH_WIRE_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

inline float OffsetValue(float& v, float offset)
{
	int sign = (v > 0) ? 1 : -1;
#if 1
	int k = abs((int)v) >> 2;
	int e = 0;
	while (k)
	{
		k >>= 2;
		e++;
	}

	while (e)
	{
		e--;
		offset = offset * 10.f;
	}
#endif
	v += sign * offset;
	return v;
}

D3DXVECTOR3& OffsetVertexPosition(D3DXVECTOR3& v, float offset)
{
	OffsetValue(v.x, offset);
	OffsetValue(v.y, offset);
	OffsetValue(v.z, offset);
	return v;
}

void RendererD3D9::Init(HWND hWnd, bool fullscreen /*=false*/)
{

	m_hWnd = hWnd;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;

	IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);


	int adapter_id = D3DADAPTER_DEFAULT;
	/*
	for (unsigned Adapter = 0; Adapter < d3d9->GetAdapterCount(); Adapter++)
	{
	MONITORINFOEX Monitor = {};
	Monitor.cbSize = sizeof(Monitor);

	if (::GetMonitorInfo(d3d9->GetAdapterMonitor(Adapter), &Monitor) && Monitor.szDevice[0])
	{
	DISPLAY_DEVICE DispDev = {};
	DispDev.cb = sizeof(DispDev);
	if (::EnumDisplayDevices(Monitor.szDevice, 0, &DispDev, 0))
	{
	if (Adapter == 0)
	{
	m_adapterFullscreenSize = Vec2i(Monitor.rcMonitor.right - Monitor.rcMonitor.left, Monitor.rcMonitor.bottom - Monitor.rcMonitor.top);
	}

	if (_tcsstr(DispDev.DeviceID, _T("OVR")))
	{
	adapter_id = Adapter;
	m_adapterFullscreenSize = Vec2i(Monitor.rcMonitor.right - Monitor.rcMonitor.left, Monitor.rcMonitor.bottom - Monitor.rcMonitor.top);
	}
	}
	}
	}*/

	D3DPRESENT_PARAMETERS pp = {};
	pp.BackBufferWidth = w;
	pp.BackBufferHeight = h;

	if (fullscreen)
	{
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		pp.BackBufferWidth = cx;
		pp.BackBufferHeight = cy;
	}

	pp.BackBufferCount = 0;
	pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	pp.AutoDepthStencilFormat = D3DFMT_D24S8;
	pp.EnableAutoDepthStencil = TRUE;
	pp.hDeviceWindow = m_hWnd;
	pp.Windowed = !fullscreen;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pp.MultiSampleQuality = 0;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	pp.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	m_d3dpp = pp;

	if (FAILED(d3d9->CreateDevice(adapter_id, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &m_pDevice)))
	{
		MessageBoxW(NULL, L"Create Device Failed.", L"Fatal Error", MB_OK);
		return;
	}
	SAFERELEASE(d3d9);

	InitState();
//	CreateResource();
}


HRESULT RendererD3D9::TestCoopLevel()
{
	HRESULT hr = S_OK;

	if (m_pDevice == NULL)
	{
		return E_FAIL;
	}

	// Check the current status of D3D9 device.
	hr = m_pDevice->TestCooperativeLevel();

	switch (hr)
	{
	case D3D_OK:
	case D3DERR_DEVICELOST:
		hr = S_OK;
		break;
	case D3DERR_DEVICENOTRESET:
		hr = ResetDevice();
		break;
	default:
		break;
	}
	return hr;
}

HRESULT RendererD3D9::ResetDevice(int w, int h, bool fullscreen)
{
	HRESULT hr = S_OK;
	if (m_pDevice)
	{
		// Check the current status of D3D9 device.
		hr = m_pDevice->TestCooperativeLevel();

		D3DPRESENT_PARAMETERS d3dpp = m_d3dpp;
		d3dpp.Windowed = !fullscreen;
		if (fullscreen)
		{
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			d3dpp.BackBufferWidth = cx;
			d3dpp.BackBufferHeight = cy;
		}
		else
		{
			RECT rc;
			::GetClientRect(m_hWnd, &rc);
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;
			d3dpp.BackBufferWidth = w;
			d3dpp.BackBufferHeight = h;
		}

		if (FAILED(m_pDevice->Reset(&d3dpp)))
		{
			Destroy();
			CreateResource();
		}
	}

	if (m_pDevice == NULL)
	{
		Init(m_hWnd);
	}

	return hr;
}

void RendererD3D9::InitState()
{
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	SetWorldMat();
	SetViewMat();
	SetProjMat(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
}

void RendererD3D9::SetWorldMat(float xAngle/*= 0.f*/, float yAngle /*= 0.f*/, float zAngle/* = 0.f*/)
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	if (xAngle != 0) D3DXMatrixRotationX(&matWorld, xAngle);
	if (yAngle != 0) D3DXMatrixRotationY(&matWorld, yAngle);
	if (zAngle != 0) D3DXMatrixRotationZ(&matWorld, zAngle);
	m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

void RendererD3D9::SetViewMat()
{
	D3DXVECTOR3 vEye(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
}

void RendererD3D9::SetProjMat(int w, int h)
{
	D3DXMATRIXA16 matProj;
	float aspect = w / h;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, aspect, 1.0f, 1000.0f);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void RendererD3D9::Update()
{
	if (m_pObjLoader->m_bLoaded)
	{
		m_bDataLoaded = true;
		CreateResource();
	}
	else
	{
		DestroyResource();
		m_bDataLoaded = false;
	}
}

void RendererD3D9::Render()
{
	if (FAILED(TestCoopLevel()))
		return;

	RenderMesh();
	Present();
}

void RendererD3D9::CreateResource()
{
	ObjLoader* loader = m_pObjLoader;
	if (m_bDataLoaded)
	{
		easymath::vector3 delta = (loader->bmax - loader->bmin) * 0.5f;
		trans = loader->bmin + delta;

		max_ext = std::max(delta.x, std::max(delta.y, delta.z));

		for each (auto o in loader->drawObjects)
		{
			MeshObject mo = {};
			mo.VertexCount = o.numTriangles * 3;
			CreateMeshGeometry(mo.pVb, mo.pWireVb, mo.VertexCount, &o.buffer.at(0));

			auto it = loader->texDatas.find(o.diffuse_tex_name);
			if (it != loader->texDatas.end())
			{
				LPDIRECT3DTEXTURE9 tex;
				TexImage &image = it->second;
				CreateMeshTexture(mo.pTex, image.m_w, image.m_h, image.m_bpp, image.m_data);
			}
			m_Meshs.push_back(mo);
		}
	}
}

inline std::wstring app_path()
{
	static bool initialized = false;
	if (!initialized)
	{
		//if (global_bUseAbsolutePath)
		{
			WCHAR bin_dir[MAX_PATH] = {};
			GetModuleFileNameW(nullptr, bin_dir, MAX_PATH);
			int len = wcslen(bin_dir);

		}

		initialized = true;
	}
	return std::wstring();
}

bool RendererD3D9::CreateMeshTexture(LPDIRECT3DTEXTURE9& tex, int w, int h, int bpp, void* data)
{
	// create surface
	DWORD usage = 0;
	D3DFORMAT fmt = (bpp == 4) ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
	m_pDevice->CreateTexture(w, h, 1, usage, fmt, D3DPOOL_DEFAULT, &tex, NULL);
	int pitch = w * bpp;
	D3DLOCKED_RECT locked;
	tex->LockRect(1, &locked, nullptr, D3DLOCK_DISCARD);

	if ((locked.Pitch == pitch) && bpp == 4)
	{
		memcpy(locked.pBits, data, pitch * h);
	}
	else
	{
		unsigned char *dst = (unsigned char *)locked.pBits;
		unsigned char *src = (unsigned char *)data;
		if (bpp == 4)
		{
			for (int i = 0; i < h; i++)
			{
				memcpy(dst, src, pitch);
				dst += locked.Pitch;
				src += pitch;
			}
		}
		else if (bpp == 3)
		{
			unsigned char *ptr1 = nullptr;
			unsigned char *ptr2 = nullptr;
			for (int i = 0; i < h; i++)
			{
				ptr1 = dst;
				ptr2 = src;
				for (int j = 0; j < w; j++)
				{
					*ptr1++ = 0xFF;
					*ptr1++ = *ptr2++;
					*ptr1++ = *ptr2++;
					*ptr1++ = *ptr2++;
				}
				dst += locked.Pitch;
				src += pitch;
			}
		}
		tex->UnlockRect(0);
	}
	return true;
}

bool RendererD3D9::CreateMeshGeometry(LPDIRECT3DVERTEXBUFFER9& pVB, LPDIRECT3DVERTEXBUFFER9& pVBwire, int count, float* data)
{
	// Create the vertex buffer.
	if (FAILED(m_pDevice->CreateVertexBuffer(count * sizeof(MESH_VERTEX), 0, D3DFVF_MESH_VERTEX, D3DPOOL_DEFAULT, &pVB, NULL)))
	{
		return false;
	}

	if (FAILED(m_pDevice->CreateVertexBuffer(count * sizeof(MESH_WIRE_VERTEX), 0, D3DFVF_MESH_WIRE_VERTEX, D3DPOOL_DEFAULT, &pVBwire, NULL)))
	{
		return false;
	}

	MESH_VERTEX* pVertices;
	if (FAILED(pVB->Lock(0, 0, (void**)&pVertices, 0)))
		return false;

	MESH_WIRE_VERTEX* pVtxsWire;
	if (FAILED(pVBwire->Lock(0, 0, (void**)&pVtxsWire, 0)))
		return false;

	float *ptr = data;
	for (DWORD i = 0; i < count; i++)
	{
		float z = *ptr++;
		float y = *ptr++;
		float x = *ptr++;
		x -= trans.x;
		y -= trans.y;
		z -= trans.z;
		//x *= 5.f / max_ext;
		//y *= 5.f / max_ext;
		//z *= 5.f / max_ext;
		pVertices[i].position = D3DXVECTOR3(x, y, z);

		pVertices[i].normal = D3DXVECTOR3(*ptr++, *ptr++, *ptr++);
		//pVertices[i].color = /*0xff800080; ptr += 3;*/D3DCOLOR_COLORVALUE(*ptr++, *ptr++, *ptr++, 0.5f);
		float r = *ptr++;
		float g = *ptr++;
		float b = *ptr++;
		pVertices[i].color = D3DCOLOR_COLORVALUE(r, g, b, 0.5f);

		pVertices[i].u = *ptr++;
		pVertices[i].v = *ptr++;

		pVtxsWire[i].position = pVertices[i].position;
		OffsetVertexPosition(pVtxsWire[i].position, 0.005f);
		pVtxsWire[i].color = D3DCOLOR_ARGB(255, 0, 0, 0);
	}
	pVB->Unlock();
	pVBwire->Unlock();
	return true;
}

void RendererD3D9::RenderMesh()
{
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(32, 64, 128), 1.0f, 0);

	if (!isResourceCreated())
		return;

	SetWorldMat(0.f, timeGetTime() / 2000.0f);

	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//SetupMatrices();

		// draw mesh obj
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		for each (auto mo in m_Meshs)
		{
			m_pDevice->SetTexture(0, mo.pTex);
			m_pDevice->SetStreamSource(0, mo.pVb, 0, sizeof(MESH_VERTEX));
			m_pDevice->SetFVF(D3DFVF_MESH_VERTEX);
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, mo.VertexCount / 3);
		}

		//draw wireframe
		m_pDevice->SetTexture(0, nullptr);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		for each (auto mo in m_Meshs)
		{
			m_pDevice->SetStreamSource(0, mo.pWireVb, 0, sizeof(MESH_WIRE_VERTEX));
			m_pDevice->SetFVF(D3DFVF_MESH_WIRE_VERTEX);
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, mo.VertexCount / 3);
		}

		m_pDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	//m_pDevice->Present(NULL, NULL, NULL, NULL);
}
