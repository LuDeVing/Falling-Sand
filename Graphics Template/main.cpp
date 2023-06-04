#include <windows.h>
#include <math.h>
#include <vector>
#include <utility>
#include <iostream>

/* Don't forget to change LINKER/SYSTEM_system from Console to Windows*/

bool running = true;

void* bufferMemory;
int bufferWidth;
int bufferHeight;
BITMAPINFO bufferBitmapInfo;

LRESULT CALLBACK windowCallBack(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        running = false;
        break;

    case WM_SIZE: {

        RECT rect;
        GetClientRect(hwnd, &rect);

        bufferWidth = rect.right - rect.left;
        bufferHeight = rect.bottom - rect.top;

        int bufferSize = bufferWidth * bufferHeight * sizeof(unsigned int);

        if (bufferMemory) VirtualFree(bufferMemory, 0, MEM_RELEASE);
        bufferMemory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        
        bufferBitmapInfo.bmiHeader.biSize = sizeof(bufferBitmapInfo.bmiHeader);
        bufferBitmapInfo.bmiHeader.biWidth = bufferWidth;
        bufferBitmapInfo.bmiHeader.biHeight = bufferHeight;
        bufferBitmapInfo.bmiHeader.biPlanes = 1;
        bufferBitmapInfo.bmiHeader.biBitCount = 32;
        bufferBitmapInfo.bmiHeader.biCompression = BI_RGB;

    }
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);

    }
    return 0;
}

void Draw(int x, int y, unsigned int color) {
    
    if (y >= bufferHeight || y < 0 || x >= bufferWidth || x < 0)
        return;

    *((unsigned int*)(bufferMemory) + y * bufferWidth + x) = color;

}


void OnStart();

void OnUpdate();

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	windowClass.lpszClassName = L"Window Class";
    windowClass.lpfnWndProc = windowCallBack;

    RegisterClass(&windowClass);

    HWND window = CreateWindow(
        windowClass.lpszClassName,
        L"NAME",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, 0, 0, hInstance, 0
    );

    HDC hdc = GetDC(window);

    OnStart();

    while (running) {

        MSG message;

        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        OnUpdate();

        StretchDIBits(hdc, 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, bufferMemory, &bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

    }

}


void OnStart() {
    
}

void OnUpdate() {

}
