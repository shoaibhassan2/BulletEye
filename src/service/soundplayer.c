#include <service/soundplayer.h>
#include <mmsystem.h>
DWORD WINAPI PlaySoundThread(LPVOID lpParameter)
{
    WORD resourceId = (WORD)(uintptr_t)lpParameter;
    HMODULE hModule = GetModuleHandleW(NULL);
    PlaySoundW(MAKEINTRESOURCEW(resourceId), hModule, SND_RESOURCE | SND_SYNC);
    return 0;
}