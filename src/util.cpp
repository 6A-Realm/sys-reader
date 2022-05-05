
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <switch.h>

#include "util.hpp"

void WriteToLog(std::string message)
{
    FILE *fp = fopen("sdmc:/sys-reader.log", "a");
    fprintf(fp, message.c_str());
    fprintf(fp, "\n");
    fclose(fp);
}


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
        R_ASSERT(smInitialize());
        R_ASSERT(fsInitialize());
        R_ASSERT(fsdevMountSdmc());
        R_ASSERT(hidInitialize());
        R_ASSERT(hidsysInitialize());
        R_ASSERT(setsysInitialize());
        SetSysFirmwareVersion fw;
        if (R_SUCCEEDED(setsysGetFirmwareVersion(&fw)))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();

        static const SocketInitConfig socketInitConfig = {
            .bsdsockets_version = 1,

            .tcp_tx_buf_size = 0x800,
            .tcp_rx_buf_size = 0x800,
            .tcp_tx_buf_max_size = 0x25000,
            .tcp_rx_buf_max_size = 0x25000,

            //We don't use UDP, set all UDP buffers to 0
            .udp_tx_buf_size = 0,
            .udp_rx_buf_size = 0,

            .sb_efficiency = 1,
        };
        R_ASSERT(socketInitialize(&socketInitConfig));   
    }

    void __appExit(void)
    {
        socketExit();
        hidsysExit();
        hidExit();
        fsdevUnmountAll();
        fsExit();;
    }
}

void handle_message(const std::string & message)
{
    WriteToLog("Recieved: " + message);
}

void buttonPressed(std::string button, json& wsMessage){
    WriteToLog(button + " pressed");
    wsMessage["data"]["pressedButtons"].push_back(button);
}