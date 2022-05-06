
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

json gatherButtonPresses(PadState pad){
    u64 kDown = padGetButtonsDown(&pad);
    json pressedButtons = json::array();

    if (kDown & HidNpadButton_A )
        buttonPressed("HidNpadButton_A", pressedButtons);

    if (kDown & HidNpadButton_B )
        buttonPressed("HidNpadButton_B", pressedButtons);
    
    if (kDown & HidNpadButton_X )
        buttonPressed("HidNpadButton_X", pressedButtons);
    
    if (kDown & HidNpadButton_Y )
        buttonPressed("HidNpadButton_Y", pressedButtons);

    if (kDown & HidNpadButton_StickL )
        buttonPressed("HidNpadButton_StickL", pressedButtons);
    
    if (kDown & HidNpadButton_StickR )
        buttonPressed("HidNpadButton_StickR", pressedButtons);

    if (kDown & HidNpadButton_L )
        buttonPressed("HidNpadButton_L", pressedButtons);

    if (kDown & HidNpadButton_R )
        buttonPressed("HidNpadButton_R", pressedButtons);
    
    if (kDown & HidNpadButton_ZL )
        buttonPressed("HidNpadButton_ZL", pressedButtons);

    if (kDown & HidNpadButton_ZR )
        buttonPressed("HidNpadButton_ZR", pressedButtons);

    if (kDown & HidNpadButton_Plus )
        buttonPressed("HidNpadButton_Plus", pressedButtons);
    
    if (kDown & HidNpadButton_Minus )
        buttonPressed("HidNpadButton_Minus", pressedButtons);

    if (kDown & HidNpadButton_Left  )
        buttonPressed("HidNpadButton_Left ", pressedButtons);
    
    if (kDown & HidNpadButton_Up )
        buttonPressed("HidNpadButton_Up", pressedButtons);

    if (kDown & HidNpadButton_Right )
        buttonPressed("HidNpadButton_Right", pressedButtons);

    if (kDown & HidNpadButton_Down )
        buttonPressed("HidNpadButton_Down", pressedButtons);

    return pressedButtons;
}

json gatherScreenPresses(s32& prev_touchcount){

    json pressedScreen = json::array();
    HidTouchScreenState state={0};
    if (hidGetTouchScreenStates(&state, 1)) {
        if (state.count != prev_touchcount)
        {
            prev_touchcount = state.count;
        }
        for(s32 i=0; i<state.count; i++)
        {
            json touchState = json::object();
            touchState["id"] = i;
            // touchState["finger_id"] = state.touches[i].finger_id;
            touchState["x"] = state.touches[i].x;
            touchState["y"] = state.touches[i].y;
            // touchState["diameter_x"] = state.touches[i].diameter_x;
            // touchState["diameter_y"] = state.touches[i].diameter_y;
            // touchState["rotation_angle"] = state.touches[i].rotation_angle;
            pressedScreen.push_back(touchState);
        }
    }
    return pressedScreen;
}

int main(int argc, char **argv)
{
    WebSocket::pointer ws = WebSocket::from_url(wsserverHost);
    bool recording = true;
    PadState pad;
    s32 prev_touchcount=0;
    
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();
    

    while (true)
    {

        padUpdate(&pad);
        
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
            wsMessage["type"] = "consoleReport";

            json pressedButtons = gatherButtonPresses(pad);
            if (pressedButtons.size() > 0)
                wsMessage["data"]["pressedButtons"] = pressedButtons;

            json pressedScreen = gatherScreenPresses(prev_touchcount);
            if (pressedScreen.size() > 0)
                wsMessage["data"]["pressedScreen"] = pressedScreen;

            if (ws &&  wsMessage.contains("data")){
                ws->send(wsMessage.dump());
            }   
        }
        svcSleepThread(1e+7);
    }
    ws->close();
    return 0;
}
