#include <iostream>
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

using namespace std;

int mains() {
    while (true) {
        POINT p;
        GetCursorPos(&p);
        std::cout << "X: " << p.x << " Y: " << p.y << "\r";
        Sleep(100);
    }
}