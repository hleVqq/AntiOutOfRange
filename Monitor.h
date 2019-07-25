#pragma once

#include <lowlevelmonitorconfigurationapi.h>

HANDLE GetPrimaryMonitor()
{
	HMONITOR handle = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTOPRIMARY);
	LPPHYSICAL_MONITOR monitors = (LPPHYSICAL_MONITOR)malloc(sizeof(PHYSICAL_MONITOR));
	HANDLE monitor = nullptr;

	if (monitors && GetPhysicalMonitorsFromHMONITOR(handle, 1, monitors))
		monitor = monitors[0].hPhysicalMonitor;

	free(monitors);

	return monitor;
}

DWORD GetMonitorSetting(HANDLE monitor, BYTE setting)
{
	DWORD value;
	GetVCPFeatureAndVCPFeatureReply(monitor, setting, NULL, &value, NULL);

	return value;
}

void SetMonitorSetting(HANDLE monitor, BYTE setting, DWORD value, DWORD sleepAfter = 0)
{
	SetVCPFeature(monitor, setting, value);
	
	if (sleepAfter)
		Sleep(sleepAfter);
}

int GetMonitorRefreshRate()
{
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	return dm.dmDisplayFrequency;
}