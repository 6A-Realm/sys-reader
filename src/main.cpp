
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <switch.h>


#define INNER_HEAP_SIZE 0x80000

extern "C"
{
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;

    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void)
    {
        void *addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        /* Newlib Heap Management */
        extern char *fake_heap_start;
        extern char *fake_heap_end;

        fake_heap_start = (char *)addr;
        fake_heap_end = (char *)addr + size;
    }

    void __appInit(void)
    {
        if (R_FAILED(smInitialize()))
        {
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
        }

        Result rc = setsysInitialize();
        if (R_SUCCEEDED(rc))
        {
            SetSysFirmwareVersion fw;
            rc = setsysGetFirmwareVersion(&fw);
            if (R_SUCCEEDED(rc))
                hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
            setsysExit();
        }


        // Disable this if you don't want to use the filesystem.
        rc = fsInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        // Disable this if you don't want to use the SD card filesystem.
        fsdevMountSdmc();
    }

    void __appExit(void)
    {
        fsdevUnmountAll(); // Disable this if you don't want to use the SD card filesystem.
        fsExit(); // Disable this if you don't want to use the filesystem.
        smExit();
    }
}

void WriteToLog(const char *message)
{
    FILE *fp = fopen("sdmc:/sys-reader.log", "a");
    fprintf(fp, message);
    fprintf(fp, "\n");
    fclose(fp);
}

int main(int argc, char **argv)
{
    WriteToLog("Starting up...");

    while (true)
    {
        svcSleepThread(1e+8L);
    }
    return 0;
}
