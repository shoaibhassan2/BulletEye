#include <globals.h>
#include <windows.h>
#include <stdio.h>
#include <gui/monitor.h>

ULONGLONG FileTimeToUlong(const FILETIME ft)
{
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

DWORD WINAPI UpdateTitleThread(LPVOID lpParam)
{

    static ULONGLONG last_total_time = 0;
    static ULONGLONG last_idle_time = 0;

    while (g_bIsRunning)
    {

        double cpu_usage = 0.0;
        FILETIME ft_idle, ft_kernel, ft_user;

        if (GetSystemTimes(&ft_idle, &ft_kernel, &ft_user))
        {
            ULONGLONG current_idle_time = FileTimeToUlong(ft_idle);
            ULONGLONG current_total_time = FileTimeToUlong(ft_kernel) + FileTimeToUlong(ft_user);

            if (last_total_time != 0)
            {
                ULONGLONG delta_total = current_total_time - last_total_time;
                ULONGLONG delta_idle = current_idle_time - last_idle_time;
                if (delta_total != 0)
                {
                    cpu_usage = (1.0 - (double)delta_idle / delta_total) * 100.0;
                }
            }
            last_total_time = current_total_time;
            last_idle_time = current_idle_time;
        }

        MEMORYSTATUSEX mem_status;
        mem_status.dwLength = sizeof(mem_status);
        GlobalMemoryStatusEx(&mem_status);

        WCHAR szTitleBuffer[128];
        swprintf_s(szTitleBuffer,
                   sizeof(szTitleBuffer) / sizeof(szTitleBuffer[0]),
                   L"%ls    CPU: %.0f%%    |     RAM: %ld%%",
                   MAIN_WINDOW_TITLE, cpu_usage, mem_status.dwMemoryLoad);

        SetWindowTextW(g_hWnd, szTitleBuffer);

        Sleep(1000);
    }
    return 0;
}