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
int delay = 0;

/*VARIABLES FOR CHANGING*/
//Minimum 6 seconds, otherwise the program will break.
int interval = 10000; //Number of milliseconds (in thousandths of seconds) per interval
//MULTIPLY THE AMOUNT OF SECONDS BY 1000
string zeroVoltage = "-0.085"; //Voltage at zero gauss
string zeroCurrent = "4.2"; //Current at zero gauss
string defaultCurrent = "4.2"; //Current used by default

//Calculate voltage from Gauss
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
	cout << "; Current: " << inputCurrent;

	//Uncomment if current needs to be changed
	/*string magVal2;
	cout << "Give me the Value for Current: ";
	cin >> magVal2;*/
	string magVal1 = inputVoltage;
	string magVal2 = inputCurrent;
	Sleep(1000);
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	//std::cout << "Hello World!\n";
	while (true) {
		Sleep(50);
		//delay += countSleep(50);
		if (GetAsyncKeyState(VK_F12)) {
			::ShowWindow(::GetConsoleWindow(), SW_HIDE); //Hides the black box

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

			/*string s = "open \"done sound effect.mp3\" type mpegvideo alias mp3";
			wstring stemp = wstring(s.begin(), s.end());
			LPCWSTR sw = stemp.c_str();
			mciSendString(sw, NULL, 0, NULL);*/

			::ShowWindow(::GetConsoleWindow(), SW_SHOW); //Shows the black box
			cout << "Give me the Value for Gauss: ";
			cin >> inputGauss;
			inputVoltage = doubleToStr(4, voltageFunction(inputGauss));
			inputCurrent = defaultCurrent;
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			keyTap(VK_TAB);
			cout << "HI\n";
		}
		if (GetAsyncKeyState(VK_NUMPAD0)) {
			terminate();
		}
		if (GetAsyncKeyState(VK_NUMPAD2)) {
			goto start;
		}
	}
}

void magnetTo(string current, string voltage) {
	delay = 0;
	/*Now we turn the magnet off*/
	{//Deletes the previous text
		keyDown(VK_CONTROL);
		clickKey('a');
		keyUp(VK_CONTROL); //Ctrl + a
		keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

		sleepCut(16);
		//Types in the voltage
		sendString(voltage);

		sleepCut(16);
		//Goes to the current box
		clickKey('\t');

		//Deletes the previous text
		keyDown(VK_CONTROL);
		clickKey('a');
		keyUp(VK_CONTROL); //Ctrl + a
		keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

		sleepCut(16);
		//Types in the current
		sendString(current);

		//Goes to and hits the magnet switch button
		clickKey('\t');
		keyTap(VK_RETURN);

		sleepCut(16);
		/*Clear the error message screen*/
		clickKey('\t');
		keyTap(VK_RETURN);

		sleepCut(16);
		/*Go back to the original location*/
		for (int tab = 0; tab < 14; tab++) {
			keyTap(VK_TAB);
		}
		sleepCut(16);
		clickKey('x'); clickKey('\b');
	}
	if (delay > interval) {
		delay = interval;
	}
	Sleep(interval - delay);
}

void clickKey(char c) {

	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = 0;
	Input.ki.wVk = VkKeyScanA(c); //Character to output
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
	Input.ki.wVk = s; //Does the command connected to the short
	Input.ki.dwFlags = 0; //Key Pressed down
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

void keyUp(short s) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = s; //Does the command connected to the short
	Input.ki.dwFlags = KEYEVENTF_KEYUP; //Key Pressed down
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

void keyTap(short s) {
	keyUp(s); keyDown(s);
}

//returns total delay caused
void sendString(string s) {
	for (int i = 0; i < s.length(); i++) {
		Sleep(10);
		clickKey(s[i]);
	}
	//return s.length() * 10;
}

void countSleep(DWORD dwMilliseconds) {
	terminateIfEsc();
	Sleep(dwMilliseconds);
	delay += dwMilliseconds;
}

void sleepCut(int percent) {
	terminateIfEsc();
	Sleep(interval * (percent) / 100);
	delay += interval * (percent) / 100;
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