// MeshObjViewer.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "MeshObjViewer.h"
#include "resource.h"
#include <commdlg.h>
#include "d3d9renderer.h"
#include "ObjLoader.h"

// Include the v6 common controls in the manifest
#pragma comment(linker, \
	"\"/manifestdependency:type='Win32' "\
	"name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' "\
	"processorArchitecture='*' "\
	"publicKeyToken='6595b64144ccf1df' "\
	"language='*'\"")

#define MAX_LOADSTRING 100

RendererD3D9 *g_pRenderer = nullptr;
ObjLoader *g_ploader = nullptr;

void OnCreate(HWND hWnd);
void OnDestroy();
void OnRender();
void OnSize(HWND hWnd, int width, int height);

BOOL FetchOpenFileName(HWND hWnd, PWSTR pszOutputName, PWSTR pszTitle = nullptr);

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MESHOBJVIEWER, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MESHOBJVIEWER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MESHOBJVIEWER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MESHOBJVIEWER));
	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MESHOBJVIEWER));
#if 0
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#endif
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			OnRender();
		}
	}
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		OnCreate(hWnd);
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
			case IDM_OPEN:
			{
				WCHAR filename[MAX_PATH] = {};
				FetchOpenFileName(hWnd, filename);
				g_ploader->Load(filename);
			}
				break;
			case IDM_CLOSE:
				g_ploader->Close();
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_SIZE:
		OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		OnDestroy();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void OnCreate(HWND hWnd)
{
	g_ploader = new ObjLoader();
	g_pRenderer = new RendererD3D9(g_ploader);
	g_pRenderer->Init(hWnd);

	//test
	//g_ploader->Load(L"1.obj");
}

void OnSize(HWND hWnd, int width, int height)
{
	g_pRenderer->Resize(width, height);
}

void OnDestroy()
{
	SAFEDELETE(g_pRenderer);
	SAFEDELETE(g_ploader);
}

void OnRender()
{
	if (g_pRenderer)
	{
		g_pRenderer->Update();
		g_pRenderer->Render();
		//g_pRenderer->Present();
	}
}

BOOL FetchOpenFileName(HWND hWnd, PWSTR pszOutputName, PWSTR pszTitle /*= nullptr*/)
{
	OPENFILENAMEW ofn = {};
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hInstance = hInst;
	ofn.hwndOwner = hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = pszOutputName;
	ofn.lpstrTitle = pszTitle;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_READONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	ofn.lpstrFile[0] = L'\0';

	if (!ofn.lpstrTitle)
	{
		ofn.lpstrTitle = L"Open an OBJ mesh file";
	}
	ofn.lpstrFilter = L"OBJ Mesh Files (*.obj)\0*.obj\0" \
			L"All Files (*.*)\0*.*\0\0";
	ofn.lpstrDefExt = L"obj";

	return GetOpenFileNameW(&ofn);
}
