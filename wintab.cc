#include <node.h>
#include <v8.h>
#include <Windows.h>
#include <iostream>

#include "WINTAB.H"

#include "Utils.h"
char* gpszProgramName = "node_wintab";

#include "MSGPACK.H"

#define PACKETDATA (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE)
#define PACKETMODE PK_BUTTONS
#include "PKTDEF.H"

using namespace v8;
using namespace std;

int pen_x = -1;
int pen_y = -1;
int pen_pressure = -1;

Handle<Value> get_pressure(const Arguments& args) {
    HandleScope scope;
    if (pen_pressure < 0)
        return scope.Close(Null());
    return scope.Close(Number::New(pen_pressure));
}

HINSTANCE hinst;
WNDCLASS wc;
HWND hwnd;

HCTX hctx;
LOGCONTEXT lc = {0};

Handle<Value> peek_message(const Arguments& args) {
    HandleScope scope;
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return scope.Close(Null());
}

HCTX initTablet(HWND hwnd) {
    AXIS TabletX = {0};
    AXIS TabletY = {0};
    AXIS Pressure = {0};
    lc.lcOptions |= CXO_SYSTEM;
    if (gpWTInfoA(WTI_DEFSYSCTX, 0, &lc) != sizeof(LOGCONTEXT))
        return (HCTX) NULL;
    if (!(lc.lcOptions & CXO_SYSTEM))
        return (HCTX) NULL;
    wsprintf(lc.lcName, "node_wintab_%x", hinst);
    lc.lcPktData = PACKETDATA;
    lc.lcOptions |= CXO_MESSAGES;
    lc.lcPktMode = PACKETMODE;
    lc.lcMoveMask = PACKETDATA;
    lc.lcBtnUpMask = lc.lcBtnDnMask;
    if (gpWTInfoA(WTI_DEVICES, DVC_X, &TabletX) != sizeof(AXIS))
        return (HCTX) NULL;
    if (gpWTInfoA(WTI_DEVICES, DVC_Y, &TabletY) != sizeof(AXIS))
        return (HCTX) NULL;
    if (gpWTInfoA(WTI_DEVICES, DVC_NPRESSURE, &Pressure) != sizeof(AXIS))
        return (HCTX) NULL;
    // test log
    cout << "pressure min: " << Pressure.axMin << ", max: " << Pressure.axMax << endl;
    //
    lc.lcInOrgX = 0;
    lc.lcInOrgY = 0;
    lc.lcInExtX = TabletX.axMax;
    lc.lcInExtY = TabletY.axMax;
    lc.lcOutOrgX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    lc.lcOutOrgY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    lc.lcOutExtX = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    lc.lcOutExtY = -GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return gpWTOpenA(hwnd, &lc, TRUE);
}

LRESULT msgLoop(HWND hwnd, unsigned msg, WPARAM wp, LPARAM lp) {
    PACKET pkt;
    switch (msg) {
    case WM_CREATE:
        hctx = initTablet(hwnd);
        break;
    case WM_NCCREATE:
        break;
    case WT_PACKET:
        if (gpWTPacket((HCTX) lp, wp, &pkt)) {
            pen_x = (int) pkt.pkX;
            pen_y = (int) pkt.pkY;
            pen_pressure = (int) pkt.pkNormalPressure;
        }
        break;
    default:
        return (LRESULT) 0L;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void init(Handle<Object> exports) {
    hinst = (HINSTANCE) GetModuleHandle(NULL);
    wc.style = 0;
    wc.lpfnWndProc = msgLoop;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
    wc.lpszMenuName =  "wintabMenu";
    wc.lpszClassName = "wintabClass";
    RegisterClass(&wc);
    LoadWintab();
    hwnd = CreateWindow(
        "wintabClass",
        "wintabWindow",
        WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL,
        (HMENU) NULL, hinst, (LPVOID) NULL
    );
    exports->Set(String::NewSymbol("pressure"), FunctionTemplate::New(get_pressure)->GetFunction());
    exports->Set(String::NewSymbol("peekMessage"), FunctionTemplate::New(peek_message)->GetFunction());
}

NODE_MODULE(wintab, init)
