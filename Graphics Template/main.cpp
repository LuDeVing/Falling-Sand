#include <windows.h>
#include <math.h>
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
#include <string>

/* Don't forget to change LINKER/SYSTEM_system from Console to Windows*/

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

bool running = true;

void* bufferMemory;

int bufferWidth = SCREENWIDTH;
int bufferHeight = SCREENHEIGHT;

BITMAPINFO bufferBitmapInfo;

HWND window;

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

float deltaTime = 0.0000000001;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	windowClass.lpszClassName = L"Window Class";
    windowClass.lpfnWndProc = windowCallBack;

    RegisterClass(&windowClass);

    window = CreateWindow(
        windowClass.lpszClassName,
        L"NAME",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SCREENWIDTH, SCREENHEIGHT, 0, 0, hInstance, 0
    );

    HDC hdc = GetDC(window);

    OnStart();

    while (running) {

        auto start = std::chrono::high_resolution_clock::now();

        MSG message;

        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        OnUpdate();

        StretchDIBits(hdc, 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, bufferMemory, &bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        deltaTime = duration.count();

        Sleep(max(16 - (int)deltaTime, 0));

        deltaTime = max(16, deltaTime);

        deltaTime /= 1000.0f;

    }

}

float gravityStrenght = 98.0f;
int screenWidth = SCREENWIDTH;
int screenHeight = SCREENHEIGHT;

enum BlockType { NONE, SOLID, SAND, WATER };

BlockType userType = SAND;

struct vec2 {
    
    float x = 0, y = 0;

    vec2();

    vec2(float x, float y){
        this->x = x;
        this->y = y;
    }

};

vec2 add(vec2 v1, vec2 v2) {
    return vec2(v1.x + v2.x, v1.y + v2.y);
}

vec2 sub(vec2 v1, vec2 v2) {
    return vec2(v1.x - v2.x, v1.y - v2.y);
}

vec2 mult(vec2 v1, vec2 v2) {
    return vec2(v1.x * v2.x, v1.y * v2.y);
}

vec2 div(vec2 v1, vec2 v2) {
    return vec2(v1.x / v2.x, v1.y / v2.y);
}

float vecLen2(vec2& v) {
    return (v.x * v.x + v.y * v.y);
}

template <typename T> T clamp(T num1, T num2, T num3) {

    T ans = num2;

    if (num1 > ans) {
        ans = num1;
        return ans;
    }

    if (num2 < ans)
        num2 = ans;

    return ans;

}

struct Block {

    vec2 velocity = vec2(0, 0);
    BlockType type = NONE;
    bool hasBeenUpdated = false;

    Block() = default;

    Block(BlockType type) {
        this->type = type;
    }

};

Block blocks[SCREENHEIGHT][SCREENWIDTH];

bool inBounds(int x, int y) {
    
    if (x < 0 || x >= SCREENWIDTH)
        return false;

    if (y < 0 || y >= SCREENHEIGHT)
        return false;
    
    return true;

}

bool isEmpty(int x, int y) {
    return (blocks[y][x].type == NONE);
}

bool isInLiquid(int x, int y, int& lx, int& ly) {

    if (inBounds(x, y) && blocks[y][x].type == WATER) {
        lx = x;
        ly = y;
        return true;
    }
    
    if (inBounds(x, y - 1) && blocks[y - 1][x].type == WATER) {
        lx = x;
        ly = y - 1;
        return true;
    }

    if (inBounds(x, y + 1) && blocks[y + 1][x].type == WATER) {
        lx = x;
        ly = y + 1;
        return true;
    }

    if (inBounds(x - 1, y) && blocks[y][x - 1].type == WATER) {
        lx = x - 1;
        ly = y;
        return true;
    }

    if (inBounds(x + 1, y) && blocks[y][x + 1].type == WATER) {
        lx = x + 1;
        ly = y;
        return true;
    }

    if (inBounds(x - 1, y - 1) && blocks[y - 1][x - 1].type == WATER) {
        lx = x - 1;
        ly = y - 1;
        return true;
    }

    if (inBounds(x - 1, y + 1) && blocks[y + 1][x - 1].type == WATER) {
        lx = x - 1;
        ly = y + 1;
        return true;
    }

    if (inBounds(x + 1, y - 1) && blocks[y - 1][x + 1].type == WATER) {
        lx = x + 1;
        ly = y - 1;
        return true;
    }

    if (inBounds(x + 1, y + 1) && blocks[y + 1][x + 1].type == WATER) {
        lx = x + 1;
        ly = y + 1;
        return true;
    }

    return false;

}

void updateSand(int x, int y) {

    Block& block = blocks[y][x];
    int fallRate = 4;

    block.velocity.y = clamp(-10.0f, float(block.velocity.y + gravityStrenght * deltaTime), 10.0f);

    block.hasBeenUpdated = true;

    if (inBounds(x, y + 1) && !isEmpty(x, y + 1)) {
        block.velocity.y /= 2.0f;
    }

    int vx = x + block.velocity.x;
    int vy = y + block.velocity.y;

    int lx, ly;

    if (inBounds(vx, vy) && (isEmpty(vx, vy)
        || (blocks[vy][vx].type == WATER && blocks[vy][vx].hasBeenUpdated
           && vecLen2(blocks[vy][vx].velocity) - vecLen2(block.velocity) > 100.0f))) {

        if (blocks[vy][vx].type == WATER) {

            int rx = rand() % 5 - 2;
            blocks[vy][vx].velocity = vec2(rx, -4.0f);

            std::swap(blocks[vy][vx], blocks[y][x]);
        
            bool loopHasEnded = false;

            for (int i = -10; i < 0; ++i) {

                for (int j = -10; j < 10; ++j) {

                    if (isEmpty(vx + j, vy + i)) {
                        std::swap(blocks[y][x], blocks[vy + i][vx + j]);
                        loopHasEnded = true;
                        break;
                    }

                }

                if (loopHasEnded)
                    break;

            }

            blocks[y][x] = Block();

        }
        else if (isEmpty(vx, vy)) {
            std::swap(blocks[vy][vx], blocks[y][x]);
        }
        
    }
    else if (inBounds(x, y + 1) && ((isEmpty(x, y + 1) || (blocks[y + 1][x].type == WATER)))) {
        block.velocity.y += (gravityStrenght * deltaTime);
        std::swap(blocks[y][x], blocks[y + 1][x]);
    }
    else if (inBounds(x - 1, y + 1) && ((isEmpty(x - 1, y + 1) || blocks[y + 1][x - 1].type == WATER))) {
        block.velocity.x = isInLiquid(x, y, lx, ly) ? 0.0f : rand() == 0 ? -1.0f : 1.0f;
        block.velocity.y += (gravityStrenght * deltaTime);
        std::swap(blocks[y][x], blocks[y + 1][x - 1]);
    }
    else if (inBounds(x + 1, y + 1) && ((isEmpty(x + 1, y + 1) || blocks[y + 1][x + 1].type == WATER))) {
        block.velocity.x = isInLiquid(x, y, lx, ly) ? 0.0f : rand() % 2 == 0 ? -1.0f : 1.0f;
        block.velocity.y += (gravityStrenght * deltaTime);
        std::swap(blocks[y][x], blocks[y + 1][x + 1]);
    }
    else if (isInLiquid(x, y, lx, ly) && rand() % 11 == 0) {
        std::swap(blocks[y][x], blocks[ly][lx]);
    }


}

void updateWater(int x, int y) {

    Block& block = blocks[y][x];
    int fallRate = 4;
    int spreadRate = 15;

    block.velocity.y = clamp(-10.0f, float(block.velocity.y + gravityStrenght * deltaTime), 10.0f);

    block.hasBeenUpdated = true;

    if (inBounds(x, y + 1) && !isEmpty(x, y + 1)) {
        block.velocity.y /= 2.0f;
    }

    float vx = block.velocity.x;
    float vy = block.velocity.y;

    int r = (rand() % 2 > 0 ? -1 : 1) * spreadRate;
    int l = -r;

    if (inBounds(x + vx, y + vy) && isEmpty(x + vx, y + vy)) {
        std::swap(blocks[y][x], blocks[int(y + vy)][int(x + vx)]);
    }
    else if (inBounds(x, y + fallRate) && isEmpty(x, y + fallRate)) {
        std::swap(blocks[y][x], blocks[y + fallRate][x]);
    }
    else if (inBounds(x + r, y + fallRate) && isEmpty(x + r, y + fallRate)) {
        std::swap(blocks[y][x], blocks[y + fallRate][x + r]);
    }
    else if (inBounds(x + l, y + fallRate) && isEmpty(x + l , y + fallRate)) {
        std::swap(blocks[y][x], blocks[y + fallRate][x + l]);
    }
    else {

        int xFound = x, yFound = y;

        for (int i = 0; i < fallRate; i++) {

            int lor = (rand() % 2 == 0 ? -1 : 1);

            for (int j = spreadRate; j > 0; j--) {

                if (inBounds(x - j * lor, y + i) && isEmpty(x - j  * lor, y + i)) {
                    xFound = x - j * lor;
                    yFound = y + i;
                    break;
                }
                if (inBounds(x + j * lor, y + i) && isEmpty(x + j * lor, y + i)) {
                    xFound = x + j * lor;
                    yFound = y + i;
                    break;
                }
            }
        }

        std::swap(blocks[y][x], blocks[yFound][xFound]);
        

    }
    

}

void getInputs() {

    if (GetKeyState(0x31) & 0x8000) {
        userType = SAND;
    }

    if (GetKeyState(0x32) & 0x8000) {
        userType = WATER;
    }

    if (GetKeyState(0x33) & 0x8000) {
        userType = SOLID;
    }

    POINT p;
    GetCursorPos(&p);

    RECT rect = { NULL };
    
    if (GetWindowRect(window, &rect)) {
        p.x -= rect.left;
        p.y -= rect.top;
    }

    if (GetAsyncKeyState(VK_LBUTTON)) {
        for (int y = p.y - 10; y < p.y + 10; y++) {
            for (int x = p.x - 10; x < p.x + 10; x++) {
                if (inBounds(x, y)) {
                    blocks[y][x].type = userType;
                }
            }
        }
    }


}

std::pair<int, int> randomPositions[SCREENHEIGHT * SCREENWIDTH];

void OnStart() {

    int randomPositiondIdx = 0;

    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            randomPositions[randomPositiondIdx] = std::pair <int, int>(x, y);
            randomPositiondIdx++;
        }
    }

    for (int k = 0; k < SCREENHEIGHT * SCREENWIDTH; k++) {
        std::swap(randomPositions[k], randomPositions[rand() % (SCREENHEIGHT * SCREENWIDTH)]);
    }

}


void OnUpdate() {

    getInputs();

    int scwtsch = SCREENHEIGHT * SCREENWIDTH;

    for (int k = 0; k < scwtsch; k++) {

        int x = randomPositions[k].first;
        int y = randomPositions[k].second;

        if (blocks[y][x].type == SAND)
            updateSand(x, y);

        if (blocks[y][x].type == WATER)
            updateWater(x, y);

    }

    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {

            blocks[y][x].hasBeenUpdated = false;

            if (blocks[y][x].type == WATER)
                Draw(x, SCREENHEIGHT - y - 1, RGB(100, 20, 0));
            else if (blocks[y][x].type == SAND)
                Draw(x, SCREENHEIGHT - y - 1, RGB(0, 100, 100));
            else if (blocks[y][x].type == SOLID)
                Draw(x, SCREENHEIGHT - y - 1, RGB(100, 100, 100));
            else Draw(x, SCREENHEIGHT - y - 1, RGB(235, 206, 135));
        
        }
    }

}
