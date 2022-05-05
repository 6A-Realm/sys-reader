
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <arpa/inet.h>
#include "util.hpp"

#include <switch.h>

#include "easywsclient.hpp"
#include "json.hpp"

using nlohmann::json;
using easywsclient::WebSocket;

std::string wsserverHost = std::string("ws://192.168.1.2:9876");
std::string username;


int main(int argc, char **argv)
{
    WriteToLog("Starting up...");

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    bool recording = true;
    WebSocket::pointer ws = WebSocket::from_url(wsserverHost);

    while (true)
    {

        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);


        if (ws){
            if(ws->getReadyState() != WebSocket::OPEN){
                ws = WebSocket::from_url(wsserverHost);
            } else {
                ws->poll();
                ws->dispatch(handle_message);
            }
        }


        if (recording)
        {

            json wsMessage;
            wsMessage["type"] = "ButtonsPressed";
            wsMessage["data"]["pressedButtons"] = json::array();

            if (kDown & HidNpadButton_A )
                buttonPressed("HidNpadButton_A", wsMessage);

            if (kDown & HidNpadButton_B )
                buttonPressed("HidNpadButton_B", wsMessage);
            
            if (kDown & HidNpadButton_X )
                buttonPressed("HidNpadButton_X", wsMessage);
            
            if (kDown & HidNpadButton_Y )
                buttonPressed("HidNpadButton_Y", wsMessage);

            if (kDown & HidNpadButton_StickL )
                buttonPressed("HidNpadButton_StickL", wsMessage);
            
            if (kDown & HidNpadButton_StickR )
                buttonPressed("HidNpadButton_StickR", wsMessage);

            if (kDown & HidNpadButton_L )
                buttonPressed("HidNpadButton_L", wsMessage);

            if (kDown & HidNpadButton_R )
                buttonPressed("HidNpadButton_R", wsMessage);
            
            if (kDown & HidNpadButton_ZL )
                buttonPressed("HidNpadButton_ZL", wsMessage);

            if (kDown & HidNpadButton_ZR )
                buttonPressed("HidNpadButton_ZR", wsMessage);

            if (kDown & HidNpadButton_Plus )
                buttonPressed("HidNpadButton_Plus", wsMessage);
            
            if (kDown & HidNpadButton_Minus )
                buttonPressed("HidNpadButton_Minus", wsMessage);

            if (kDown & HidNpadButton_Left  )
                buttonPressed("HidNpadButton_Left ", wsMessage);
            
            if (kDown & HidNpadButton_Up )
                buttonPressed("HidNpadButton_Up", wsMessage);

            if (kDown & HidNpadButton_Right )
                buttonPressed("HidNpadButton_Right", wsMessage);

            if (kDown & HidNpadButton_Down )
                buttonPressed("HidNpadButton_Down", wsMessage);
            

            if (ws && wsMessage["data"]["pressedButtons"].size() > 0)
            {
                ws->send(wsMessage.dump());
                WriteToLog(wsMessage.dump());
            }
                
        }

        svcSleepThread(1e+7);
    }
    return 0;
}
