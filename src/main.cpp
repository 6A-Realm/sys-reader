
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

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
        rc = fsInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));
        fsdevMountSdmc();

        rc = hidInitialize();
        if (R_FAILED(rc))
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));
    }

    void __appExit(void)
    {
        hidExit();
        fsdevUnmountAll();
        fsExit();
        smExit();
    }
}

void WriteToLog(std::string message)
{
    FILE *fp = fopen("sdmc:/sys-reader.log", "a");
    fprintf(fp, message.c_str());
    fprintf(fp, "\n");
    fclose(fp);
}


int main(int argc, char **argv)
{
    WriteToLog("Starting up...");

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    bool recording = true;

    while (true)
    {

        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus && kDown & HidNpadButton_Minus)
        {
            recording = !recording;
            WriteToLog("Toggled recording to logfile");
            
        }

        if (recording)
        {
            if (kDown & HidNpadButton_A )
            {
                WriteToLog("A pressed");
            }

            if (kDown & HidNpadButton_B )
            {
                WriteToLog("B pressed");
            }
        }

        svcSleepThread(1e+8L);
    }
    return 0;
}
