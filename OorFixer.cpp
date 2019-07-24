// OorFixer.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "OorFixer.h"
#include <lowlevelmonitorconfigurationapi.h>
#include <map>
//#include <FwCommon.h>
#include <ChString.h>
#include <shellapi.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND Window;
HANDLE Monitor;

BYTE MonitorSettingsToLoad[] =
{
	0xCC, // OSD Language
	0xDC  // Display Mode
};

std::map<BYTE, DWORD> MonitorSettings;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HANDLE GetPrimaryMonitor()
{
	HMONITOR handle = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTOPRIMARY);
	LPPHYSICAL_MONITOR monitors = (LPPHYSICAL_MONITOR)malloc(sizeof(PHYSICAL_MONITOR));
	HANDLE monitor = NULL;

	if (monitors && GetPhysicalMonitorsFromHMONITOR(handle, 1, monitors))
		monitor = monitors[0].hPhysicalMonitor;

	free(monitors);
	
	return monitor;
}

void LoadMonitorSettings()
{
	for (BYTE setting : MonitorSettingsToLoad)
	{
		DWORD value;
		GetVCPFeatureAndVCPFeatureReply(Monitor, setting, NULL, &value, NULL);
		MonitorSettings[setting] = value;
	}
}

int GetMonitorRefreshRate()
{
	DEVMODE settings;
	settings.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &settings);

	return settings.dmDisplayFrequency;
}

void ApplyMonitorSettings()
{
	for (BYTE setting : MonitorSettingsToLoad)
	{
		SetVCPFeature(Monitor, setting, MonitorSettings[setting]);
		Sleep(250);
	}
}

void tt() {

	NOTIFYICONDATA Tray;
	HWND hWnd = Window;

	//window handle
	//hWnd = FindWindow("ConsoleWindowClass", NULL);

	//hide the window
	ShowWindow(hWnd, 0);

	//tray info
	Tray.cbSize = sizeof(Tray);
	Tray.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	Tray.hWnd = hWnd;
	//strcpy(Tray.szTip, "My Application");
	lstrcpyW(Tray.szTip, L"OOR Fixer");
	//Tray.uCallbackMessage = WM_LBUTTONDOWN;
	Tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Tray.uID = 1;

	//set the icon in tasbar tray
	Shell_NotifyIcon(NIM_ADD, &Tray);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	//tt();

	Monitor = GetPrimaryMonitor();
	LoadMonitorSettings();

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	lstrcpyW(szTitle, L"OOR Fixer");
    LoadStringW(hInstance, IDC_OORFIXER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OORFIXER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OORFIXER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OORFIXER);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   Window = hWnd;

   //ShowWindow(hWnd, nCmdShow);
   //ShowWindow(Window, SW_SHOWMINIMIZED);
   //UpdateWindow(hWnd);

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
	case WM_DISPLAYCHANGE:
	{
		if (GetMonitorRefreshRate() > 144)
		{
			Sleep(2500);
			ApplyMonitorSettings();
		}
	}
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
