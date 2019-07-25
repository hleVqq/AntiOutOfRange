#include <Windows.h>
#include <tchar.h>

#include "Monitor.h"

enum
{
	VCP_OSD_LANGUAGE = 0xCC,
	VCP_PICTURE_MODE = 0xDC,
	VCP_AMA = 0xF0
};

HANDLE Monitor;
DWORD OsdLanguage, PictureMode, Ama;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DISPLAYCHANGE:
		if (GetMonitorRefreshRate() > 144)
		{
			Sleep(2500);
			SetMonitorSetting(Monitor, VCP_OSD_LANGUAGE, OsdLanguage, 250);
			SetMonitorSetting(Monitor, VCP_PICTURE_MODE, PictureMode, 250);
			SetMonitorSetting(Monitor, VCP_AMA, Ama);
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
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("OOR Fixer"), NULL);

		return 1;
	}

	HWND window = CreateWindow(L"OorFixer", L"OOR Fixer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, instance, NULL);

	if (!window)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("OOR Fixer"), NULL);

		return 1;
	}

	return 0;
}

int CALLBACK WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int cmdShow)
{
	Monitor = GetPrimaryMonitor();
	OsdLanguage = GetMonitorSetting(Monitor, VCP_OSD_LANGUAGE);
	PictureMode = GetMonitorSetting(Monitor, VCP_PICTURE_MODE);
	Ama = GetMonitorSetting(Monitor, VCP_AMA);

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