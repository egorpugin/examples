#include <windows.h>

extern "C" void doMallocReplacement();

#ifdef _WINDLL
extern "C" BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
extern "C" BOOL WINAPI __tbb_entry_point(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (callReason == DLL_PROCESS_ATTACH && reserved && hInst)
    {
        if (!GetEnvironmentVariable("TBB_MALLOC_DISABLE_REPLACEMENT", 0, 0))
            doMallocReplacement();
    }
    return DllMainCRTStartup(hinstDLL, fdwReason, lpReserved);
}
#else
extern "C" int mainCRTStartup();
extern "C" int __tbb_entry_point()
{
    if (!GetEnvironmentVariable("TBB_MALLOC_DISABLE_REPLACEMENT", 0, 0))
        doMallocReplacement();
    return mainCRTStartup();
}
#endif
