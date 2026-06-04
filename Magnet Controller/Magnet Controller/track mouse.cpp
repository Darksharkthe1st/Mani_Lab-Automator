#include <iostream>
#include <Windows.h>

int main() {
    while (true) {
        POINT p;
        GetCursorPos(&p);
        std::cout << "X: " << p.x << " Y: " << p.y << "\r";
        Sleep(100);
    }
}