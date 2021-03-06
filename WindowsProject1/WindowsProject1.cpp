// WindowsProject1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WindowsProject1.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d3d9.h>
#include <math.h>

// include the Direct3D library file
#pragma comment(lib,"d3d9.lib")

// Define program window's sizes
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define COLORREF_BASE 256

#define MAX_LOADSTRING 100

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct ColorVertex
{
	float x, y, z, rhw;
	D3DCOLOR color;	
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

LPDIRECT3D9 d3d;					// the pointer to Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;		// the pointer to device class
LPDIRECT3DVERTEXBUFFER9 Triangle; // Vertex
D3DMATRIX worldMatrix;

int starting_time;			// global timer 

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

D3DCOLOR random_color(void);	// function to get random color

void initD3D(HWND hWnd);			// setup and initializes Direct3D
void render_frame(D3DCOLOR A, D3DCOLOR B, D3DCOLOR C);			// renders a single frame
void cleanD3D(void);					// closes Direct3D and releases memory
void createVertex(int x, int y, D3DCOLOR color1, D3DCOLOR color2);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);    

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	 srand(GetTickCount());

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }    
    MSG msg;

	 // Choose 3 colors A,B,C
	 D3DCOLOR colorA = random_color();		// color A
	 D3DCOLOR colorB = random_color();		// color B
	 D3DCOLOR colorC = random_color();		// color C

    // Main message loop:	 
	 while (TRUE)
	 {
		 if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		 {
			 if (msg.message == WM_QUIT)
				 break;

			 TranslateMessage(&msg);
			 DispatchMessage(&msg);
		 }
		 render_frame(colorA, colorB, colorC);	// display the frame with 3 colors above
	 }

	 // clean up DirectX and COM
	 cleanD3D();
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

	 ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(
		szWindowClass, 
		szTitle, 
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
      CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH, SCREEN_HEIGHT, 
		NULL, 
		NULL, 
		hInstance, 
		NULL
	);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

	// start the timer
	starting_time = GetTickCount() / 1000;
	
	// setup and initialize Direct3D
	initD3D(hWnd);

   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...				
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp; // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp)); // clear out the struct for use
	
	d3dpp.Windowed = TRUE; // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
	d3dpp.hDeviceWindow = hWnd; // set the window to be used by Direct3D

	// create a device class using this information and the info from the d3dpp struct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							hWnd,
							D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							&d3dpp,
							&d3ddev);
	return;
}

// this is the function used to render a single frame
void render_frame(D3DCOLOR A, D3DCOLOR B, D3DCOLOR C)
{
	int param = GetTickCount() / 1000 - starting_time;			// get the time elapsed 	
	double result = round(sin(param * 2) * 10) / 10;			// get the value of sine function correct to one decimal place
	
	D3DRECT rect1 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2 };
	D3DRECT rect2 = { 0, SCREEN_HEIGHT/2 , SCREEN_WIDTH, SCREEN_HEIGHT };
	
	// debug purpose
	wchar_t text_buffer[20] = { 0 };
	OutputDebugString(L" Time used: ");
	swprintf(text_buffer, _countof(text_buffer), L"%d", param); // convert
	OutputDebugString(text_buffer); // print
	OutputDebugString(L" chill \n Result: ");
	swprintf(text_buffer, _countof(text_buffer), L"%.1f", result); // convert
	OutputDebugString(text_buffer); // print
	OutputDebugString(L" \n ");

	d3ddev->BeginScene();	// begins the 3D scene
	// start the 3D rendering
	
	// fill background with color A if sine value is -1
	if (result == -1.0)
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, A, 1.0f, 0);

	// fill background with color B if sine value is 0
	if (result == 0.0)
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, B, 1.0f, 0);

	// fill background with color C if sine value is 1
	if (result == 1.0)
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, C, 1.0f, 0);

	// fill background with 50% color B and 50% color C if sine value is 0.5
	if (result == 0.5) 
	{
		d3ddev->Clear(1, &rect1, D3DCLEAR_TARGET, B, 0, 0);
		d3ddev->Clear(1, &rect2, D3DCLEAR_TARGET, C, 0, 0);		
	}	

	if ((result > 0 ) && (result < 1))
		createVertex(SCREEN_WIDTH, SCREEN_HEIGHT, B, C);
	if ((result > -1) && (result < 0))
		createVertex(SCREEN_WIDTH, SCREEN_HEIGHT, A, B);

	// fill background with 50% color A and 50% color B if sine value is -0.5
	if (result == -0.5)
	{
		d3ddev->Clear(1, &rect1, D3DCLEAR_TARGET, A, 0, 0);
		d3ddev->Clear(1, &rect2, D3DCLEAR_TARGET, B, 0, 0);		
	}

	d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen

	return;
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	Triangle->Release(); // close and release the triangle
	d3ddev->Release(); // close and release the 3D device
	d3d->Release(); // close and release Direct3D
	return;
}


void createVertex(int x, int y, D3DCOLOR color1, D3DCOLOR color2)
{
	// fill the buffers with the rectangle data
	ColorVertex vertices[] =
	{
		{0, 0, 0, 1.0f, color1,},

		{x, 0, 0, 1.0f, color1,},

		{0, y, 0, 1.0f, color2,},

		{x, y, 0, 1.0f, color2,},
	};
		
	// create vertex buffer
	d3ddev->CreateVertexBuffer(4 * sizeof(ColorVertex),
									NULL,
									CUSTOMFVF,
									D3DPOOL_MANAGED,
									&Triangle, 
									NULL);
	
	VOID* pVoid;	// void pointer

	Triangle->Lock(0, 0, (void**)&pVoid, 0);		// lock the Triangle
	memcpy(pVoid, vertices, sizeof(vertices));	// copy the vertices to the locked buffer
	Triangle->Unlock();	// unlock the triangle	
	
	// select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	// select the triangle to display
	d3ddev->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));
	d3ddev->SetTransform(D3DTS_WORLD, &worldMatrix);
	d3ddev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	// copy the triangle to the back buffer
	d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	return;
}

// this function to get a random color
D3DCOLOR random_color(void)
{	
	BYTE x, y, z;
	x = rand() % COLORREF_BASE;
	y = rand() % COLORREF_BASE;
	z = rand() % COLORREF_BASE;

	return D3DCOLOR_XRGB(x, y, z);
}