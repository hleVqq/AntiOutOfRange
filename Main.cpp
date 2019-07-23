#include <stdio.h>
#include <Windows.h>
#include <lowlevelmonitorconfigurationapi.h>

int main()
{
	HMONITOR handle = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTOPRIMARY);
	LPPHYSICAL_MONITOR monitors = (LPPHYSICAL_MONITOR)malloc(sizeof(PHYSICAL_MONITOR));

	HANDLE monitor = NULL;

	if (monitors && GetPhysicalMonitorsFromHMONITOR(handle, 1, monitors))
	{
		monitor = monitors[0].hPhysicalMonitor;

		DWORD cc = 0, dc = 0;
		GetVCPFeatureAndVCPFeatureReply(monitor, 0xCC, NULL, &cc, NULL); printf("0xCC = %d\n", cc);
		GetVCPFeatureAndVCPFeatureReply(monitor, 0xDC, NULL, &dc, NULL); printf("0xDC = %d\n", dc);

		int sleep;
		printf("Apply fix after how many seconds?\n");
		scanf_s("%d", &sleep);

		printf("Applying fix in %d seconds...\n", sleep);
		Sleep(sleep * 1000);

		SetVCPFeature(monitor, 0xCC, cc);
		Sleep(1000);
		SetVCPFeature(monitor, 0xDC, dc);
	}

	free(monitors);

	return 0;
}