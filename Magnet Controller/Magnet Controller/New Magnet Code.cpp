#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <string>
#pragma comment(lib, "Winmm.lib")

using namespace std;

void clickKey(char c);
void keyDown(short s);
void keyUp(short s);
void keyTap(short s);
void sendString(string s);
void countSleep(DWORD milliseconds);
void sleepCut(int percent);
string doubleToStr(int decimalPlaces, double d);
void terminateIfEsc();
void magnetTo(string current, string voltage);
void clickAt(POINT p);
void dismissDialog();

int delay = 0;

/*VARIABLES FOR CHANGING*/
// Minimum 6 seconds, otherwise the program will break.
int interval = 10000; // milliseconds per interval
string zeroVoltage = "-0.085"; // Voltage at zero gauss
string zeroCurrent = "4.2";    // Current at zero gauss
string defaultCurrent = "4.2"; // Current used by default

// Offsets from SET button to each field (based on lab measurements)
// SET is at X=732, VSET center is at X=639, ISET center is at X=831
// So VSET is 93px left of SET, ISET is 99px right of SET
const int VSET_OFFSET_X = -93;
const int ISET_OFFSET_X = 99;
const int FIELD_OFFSET_Y = 0; // All fields are on the same row as SET

// Recorded position of the SET button — filled in when user presses F12
POINT setButtonPos;

// Calculate voltage from Gauss
double voltageFunction(int gauss) {
	return (gauss - 21.1) / 252.34;
}

int main()
{
start:
	string inputVoltage;
	string endingVoltage;
	double inputGauss;
	string binaryString;
	double endingGauss;

	// Ask user to place mouse on SET button and press F12 to record position
	cout << "Place your mouse cursor directly on the SET button in Kepco, then press F12.\n";
	while (true) {
		if (GetAsyncKeyState(VK_F12) & 0x8000) {
			GetCursorPos(&setButtonPos);
			cout << "SET button recorded at: " << setButtonPos.x << ", " << setButtonPos.y << "\n";
			Sleep(300); // debounce
			break;
		}
		Sleep(50);
	}

	cout << "Give me the Value for Gauss: ";
	cin >> inputGauss;
	cout << "Give me your interval in milliseconds (must be greater than 7500): ";
	cin >> interval;
	cout << "Give me the binary string (1's and 0's only): ";
	cin >> binaryString;
	cout << "Give me the final value for Gauss: ";
	cin >> endingGauss;

	inputVoltage = doubleToStr(0, voltageFunction(inputGauss));
	endingVoltage = doubleToStr(0, voltageFunction(endingGauss));

	string inputCurrent = defaultCurrent;
	cout << "Voltage: " << inputVoltage;
	cout << "; Current: " << inputCurrent << "\n";

	Sleep(1000);

	while (true) {
		Sleep(50);
		if (GetAsyncKeyState(VK_F12) & 0x8000) {
			::ShowWindow(::GetConsoleWindow(), SW_HIDE);

			for (int i = 0; i < binaryString.length(); i++) {
				if (binaryString[i] == '0') {
					magnetTo(zeroCurrent, zeroVoltage);
				}
				else if (binaryString[i] == '1') {
					magnetTo(inputCurrent, inputVoltage);
				}
				else if (binaryString[i] == '2') {
					magnetTo(inputCurrent, endingVoltage);
				}
			}

			magnetTo(inputCurrent, endingVoltage);

			::ShowWindow(::GetConsoleWindow(), SW_SHOW);
			cout << "Give me the Value for Gauss: ";
			cin >> inputGauss;
			inputVoltage = doubleToStr(4, voltageFunction(inputGauss));
			inputCurrent = defaultCurrent;

			break;
		}
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			keyTap(VK_TAB);
			cout << "HI\n";
		}
		if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) {
			terminate();
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) {
			goto start;
		}
	}
}

void magnetTo(string current, string voltage) {
	delay = 0;

	// Build field positions from the recorded SET button position
	POINT vsetPos, isetPos, setPos;
	vsetPos.x = setButtonPos.x + VSET_OFFSET_X;
	vsetPos.y = setButtonPos.y + FIELD_OFFSET_Y;
	isetPos.x = setButtonPos.x + ISET_OFFSET_X;
	isetPos.y = setButtonPos.y + FIELD_OFFSET_Y;
	setPos = setButtonPos;

	// --- Dismiss any Kepco dialog before starting ---
	dismissDialog();

	// --- Click into VSET and type voltage ---
	clickAt(vsetPos);
	countSleep(50);
	keyDown(VK_CONTROL);
	clickKey('a');
	keyUp(VK_CONTROL);
	keyDown(VK_DELETE);
	countSleep(16);
	sendString(voltage);
	countSleep(16);

	// --- Click into ISET and type current ---
	clickAt(isetPos);
	countSleep(50);
	keyDown(VK_CONTROL);
	clickKey('a');
	keyUp(VK_CONTROL);
	keyDown(VK_DELETE);
	countSleep(16);
	sendString(current);
	countSleep(16);

	// --- Click SET button ---
	clickAt(setPos);
	countSleep(16);

	// --- Dismiss any dialog that appeared after SET ---
	dismissDialog();

	// --- Return focus to VSET so next iteration starts consistently ---
	clickAt(vsetPos);
	countSleep(16);

	if (delay > interval) {
		delay = interval;
	}
	Sleep(interval - delay);
}

void dismissDialog() {
	POINT dialogButton;
	dialogButton.x = 690;
	dialogButton.y = 3922;
	clickAt(dialogButton);
	Sleep(50);
}

void clickAt(POINT p) {
	SetCursorPos(p.x, p.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(30);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void clickKey(char c) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = 0;
	Input.ki.wVk = VkKeyScanA(c);
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));

	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

void keyDown(short s) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = s;
	Input.ki.dwFlags = 0;
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

void keyUp(short s) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = s;
	Input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

void keyTap(short s) {
	keyUp(s); keyDown(s);
}

void sendString(string s) {
	for (int i = 0; i < s.length(); i++) {
		Sleep(10);
		clickKey(s[i]);
	}
}

void countSleep(DWORD dwMilliseconds) {
	terminateIfEsc();
	Sleep(dwMilliseconds);
	delay += dwMilliseconds;
}

void sleepCut(int percent) {
	terminateIfEsc();
	Sleep(interval * percent / 100);
	delay += interval * percent / 100;
}

string doubleToStr(int decimalPlaces, double d) {
	d = round(d * 10000) / 10000;
	string str = to_string(d);
	while (str[str.length() - 1] == '0') {
		str = str.substr(0, str.length() - 1);
	}
	return str;
}

void terminateIfEsc() {
	if (GetAsyncKeyState(VK_ESCAPE)) {
		terminate();
	}
}