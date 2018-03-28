#pragma once
#include "pch.h"
#include <d3d9.h>
#include "easymath.h"

#include "..\\d3dx9\\include\\d3dx9.h"
#ifdef _WIN64
#pragma comment (lib, "..\\d3dx9\\Lib\\x64\\d3dx9.lib")
#else
#pragma comment (lib, "..\\d3dx9\\Lib\\x86\\d3dx9.lib")
#endif

class ObjLoader;

struct MeshObject
{
	int VertexCount;
	LPDIRECT3DVERTEXBUFFER9 pVb;
	LPDIRECT3DVERTEXBUFFER9 pWireVb;
	LPDIRECT3DTEXTURE9		pTex;
};
class RendererD3D9
{
public:
	RendererD3D9(ObjLoader* loader) : m_pDevice(nullptr), m_bDataLoaded(false), m_pObjLoader(loader)
	{
		m_Meshs.clear();
	}
	~RendererD3D9()
	{
		Destroy();
	}
	virtual void Init(HWND hWnd, bool fullscreen = false);
	virtual void Destroy()
	{
		DestroyResource();
		SAFERELEASE(m_pDevice);
	}
	virtual HRESULT ResetDevice(int w = 0, int h = 0, bool fullscreen = false);
	virtual void Resize(int w, int h)
	{
		// not recreate device here.
		SetProjMat(w, h);
	}

	virtual void Update();
	virtual void Render();

	virtual void Present()
	{
		m_pDevice->Present(NULL, NULL, NULL, NULL);
	}
protected:
	virtual HRESULT TestCoopLevel();
	virtual void InitState();
	virtual void CreateResource();
	virtual void DestroyResource()
	{
		for each (auto m in m_Meshs)
		{
			SAFERELEASE(m.pVb);
			SAFERELEASE(m.pWireVb);
			SAFERELEASE(m.pTex);
		}
		m_Meshs.clear();
	}
	virtual bool isResourceCreated()
	{
		return (!m_Meshs.empty());
	}

	virtual bool CreateMeshTexture(LPDIRECT3DTEXTURE9& tex, int w, int h, int bpp, void* data);
	virtual bool CreateMeshGeometry(LPDIRECT3DVERTEXBUFFER9& pVB, LPDIRECT3DVERTEXBUFFER9& pVBwire, int count, float* data);
	virtual void RenderMesh();
	virtual void SetWorldMat(float =0.f, float yAngle=0.f, float zAngle=0.f);
	virtual void SetViewMat();
	virtual void SetProjMat(int w, int h);
protected:
	HWND m_hWnd;
	IDirect3DDevice9 *m_pDevice;
	D3DPRESENT_PARAMETERS m_d3dpp;
	ObjLoader* m_pObjLoader;

	std::vector<MeshObject> m_Meshs;
	bool m_bDataLoaded;

	easymath::vector3 trans;
	float max_ext;
};
