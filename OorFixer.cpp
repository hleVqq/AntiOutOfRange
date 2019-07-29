#include <Windows.h>
#include <tchar.h>

#include "OorFixer.h"
#include "Monitor.h"

#define DELAY_INITIAL 2500
#define DELAY_SETTING 250

enum VcpCode
{
	VCP_OSD_LANGUAGE = 0xCC,
	VCP_PICTURE_MODE = 0xDC,
	VCP_AMA = 0xF0
};

HANDLE Monitor;

DWORD OriginalOsdLanguage;
DWORD OriginalPictureMode;
DWORD OriginalAma;

int InitialDelay = DELAY_INITIAL;
int SettingDelay = DELAY_SETTING;

void RestoreOriginalMonitorSettings()
{
	Sleep(InitialDelay);
	SetVCPFeature(Monitor, VCP_OSD_LANGUAGE, OriginalOsdLanguage); Sleep(SettingDelay);
	SetVCPFeature(Monitor, VCP_PICTURE_MODE, OriginalPictureMode); Sleep(SettingDelay);
	SetVCPFeature(Monitor, VCP_AMA, OriginalAma);
}

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DISPLAYCHANGE:
		if (GetMonitorRefreshRate() > 144)
			RestoreOriginalMonitorSettings();

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
	const LPCWSTR CLASS_NAME = L"OorFixer";

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
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T(WINDOW_TITLE), NULL);

		return 1;
	}

	HWND window = CreateWindow(CLASS_NAME, _T(WINDOW_TITLE), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, instance, NULL);

	if (!window)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T(WINDOW_TITLE), NULL);

		return 1;
	}

	return 0;
}

void CacheInitialMonitorSettings()
{
	OriginalOsdLanguage = GetMonitorSetting(Monitor, VCP_OSD_LANGUAGE);
	OriginalPictureMode = GetMonitorSetting(Monitor, VCP_PICTURE_MODE);
	OriginalAma = GetMonitorSetting(Monitor, VCP_AMA);

	if (OriginalOsdLanguage > 100)
		OriginalOsdLanguage = 2;

	if (OriginalPictureMode > 100)
		OriginalPictureMode = 0;

	if (OriginalAma > 100)
		OriginalAma = 1;
}

int CALLBACK WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int cmdShow)
{
	Monitor = GetPrimaryMonitor();
	CacheInitialMonitorSettings();

	int argCount;
	LPWSTR *args = CommandLineToArgvW(GetCommandLine(), &argCount);

	if (args && argCount > 1)
	{
		InitialDelay = (int)wcstod(args[1], _T('\0'));

		if (argCount > 2)
			SettingDelay = (int)wcstod(args[2], _T('\0'));
	}

	if (GetMonitorRefreshRate() > 144)
		RestoreOriginalMonitorSettings();

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