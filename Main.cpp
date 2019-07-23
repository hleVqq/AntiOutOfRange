#include <stdio.h>
#include <Windows.h>
#include <lowlevelmonitorconfigurationapi.h>

int main()
{
	HMONITOR handle = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTOPRIMARY);
	LPPHYSICAL_MONITOR monitors = (LPPHYSICAL_MONITOR)malloc(sizeof(PHYSICAL_MONITOR));

	HANDLE monitor = NULL;
	DWORD initialCc = 0, initialDc = 0;

	if (monitors && GetPhysicalMonitorsFromHMONITOR(handle, 1, monitors))
	{
		monitor = monitors[0].hPhysicalMonitor;

		GetVCPFeatureAndVCPFeatureReply(monitor, 0xCC, NULL, &initialCc, NULL); printf("0xCC = %d\n", initialCc);
		GetVCPFeatureAndVCPFeatureReply(monitor, 0xDC, NULL, &initialDc, NULL); printf("0xDC = %d\n", initialDc);
	}

	free(monitors);
	printf("Apply fix after how many seconds?\n");

	int sleep;

	do
	{
		scanf_s("%d", &sleep);
	} 
	while (sleep < 1 || sleep > INT_MAX);
	
	printf("Applying fix in %d seconds...\n", sleep);
	Sleep(sleep * 1000);

	DWORD cc, dc;
	GetVCPFeatureAndVCPFeatureReply(monitor, 0xCC, NULL, &cc, NULL);
	GetVCPFeatureAndVCPFeatureReply(monitor, 0xDC, NULL, &dc, NULL);

	SetVCPFeature(monitor, 0xCC, initialCc);
	Sleep(1000);
	SetVCPFeature(monitor, 0xDC, initialDc);

	return 0;
}