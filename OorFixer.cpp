#include <Windows.h>
#include <tchar.h>

#include "Monitor.h"

const BYTE OSD_LANGUAGE = 0xCC;
const BYTE DISPLAY_MODE = 0xDC;

HANDLE Monitor;
DWORD OsdLanguage, DisplayMode;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DISPLAYCHANGE:
		if (GetMonitorRefreshRate() > 144)
		{
			Sleep(2500);
			SetMonitorSetting(Monitor, OSD_LANGUAGE, OsdLanguage, 250);
			SetMonitorSetting(Monitor, DISPLAY_MODE, DisplayMode);
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);

		break;
	default:
		return DefWindowProc(window, message, wParam, lParam);

		break;
	}

	return 0;
}

int SetupWindow(HINSTANCE instance, int cmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"OorFixer";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);

		return 1;
	}

	HWND window = CreateWindow(L"OorFixer", L"OOR Fixer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, instance, NULL);

	if (!window)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);

		return 1;
	}

	return 0;
}

int CALLBACK WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int cmdShow)
{
	Monitor = GetPrimaryMonitor();
	OsdLanguage = GetMonitorSetting(Monitor, OSD_LANGUAGE);
	DisplayMode = GetMonitorSetting(Monitor, DISPLAY_MODE);

	if (SetupWindow(instance, cmdShow))
		return 1;

	MSG message;

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return (int)message.wParam;
}