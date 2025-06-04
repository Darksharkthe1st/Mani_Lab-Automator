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
string doubleToStr(int decimalPlaces, double d);
void terminateIfEsc();
int delay = 0;

/*VARIABLES FOR CHANGING*/
//Minimum 6 seconds, otherwise the program will break.
int interval = 10000; //Number of milliseconds (in thousandths of seconds) per interval
//MULTIPLY THE AMOUNT OF SECONDS BY 1000
string zeroVoltage = "0.071"; //Voltage at zero gauss

string zeroCurrent = "4"; //Current at zero gauss


int main()
{
start:
	/*string s = "open \"done sound effect.mp3\" type mpegvideo alias mp3";
	wstring stemp = wstring(s.begin(), s.end());
	LPCWSTR sw = stemp.c_str();
	mciSendString(sw, NULL, 0, NULL);*/
	string inputVoltage;
	double inputGauss;
	cout << "Give me the Value for Gauss: ";
	cin >> inputGauss;
	inputVoltage = doubleToStr(0, (inputGauss + 9.0991) / 260.65);
	string inputCurrent = "4";
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
			for (int e = 0; e < 6;) {
				terminateIfEsc();
				system("CLS");
				cout << "Off:\n";
				cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
				cout << "Voltage: " + zeroVoltage; cout << "; Current: 4\n";
				cout << "On:\n";
				cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
				cout << "Voltage: " + inputVoltage; cout << "; Current: 4\n";
				cout << "Current State: ";
				delay = 0;
				if (magVal1 == zeroVoltage) {
					magVal1 = inputVoltage;
					magVal2 = inputCurrent;
					e++;
					cout << "on\n";
				}
				else {
					magVal1 = zeroVoltage;
					magVal2 = zeroCurrent;
					cout << "off\n";
				}
				delay = 0;
				//Deletes the previous text
				keyDown(VK_CONTROL);
				clickKey('a');
				keyUp(VK_CONTROL); //Ctrl + a
				keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

				countSleep(1000);
				//Types in the voltage
				sendString(magVal1);

				countSleep(1000);
				//Goes to the current box
				clickKey('\t');

				//Deletes the previous text
				keyDown(VK_CONTROL);
				clickKey('a');
				keyUp(VK_CONTROL); //Ctrl + a
				keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

				countSleep(1000);
				//Types in the current
				sendString(magVal2);

				//Goes to and hits the magnet switch button
				clickKey('\t');
				keyTap(VK_RETURN);

				countSleep(1000);
				/*Clear the error message screen*/
				clickKey('\t');
				keyTap(VK_RETURN);

				countSleep(1000);
				/*Go back to the original location*/
				for (int tab = 0; tab < 14; tab++) {
					keyTap(VK_TAB);
				}
				countSleep(1000);
				clickKey('x'); clickKey('\b');
				terminateIfEsc();
				Sleep(interval - delay);
			}

			/*Now we turn the magnet off*/
			{//Deletes the previous text
				keyDown(VK_CONTROL);
				clickKey('a');
				keyUp(VK_CONTROL); //Ctrl + a
				keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

				countSleep(1000);
				//Types in the voltage
				sendString(zeroVoltage);

				countSleep(1000);
				//Goes to the current box
				clickKey('\t');

				//Deletes the previous text
				keyDown(VK_CONTROL);
				clickKey('a');
				keyUp(VK_CONTROL); //Ctrl + a
				keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete

				countSleep(1000);
				//Types in the current
				sendString(zeroCurrent);

				//Goes to and hits the magnet switch button
				clickKey('\t');
				keyTap(VK_RETURN);

				countSleep(1000);
				/*Clear the error message screen*/
				clickKey('\t');
				keyTap(VK_RETURN);

				countSleep(1000);
				/*Go back to the original location*/
				for (int tab = 0; tab < 14; tab++) {
					keyTap(VK_TAB);
				}
				countSleep(1000);
				clickKey('x'); clickKey('\b');
			}
			string s = "open \"done sound effect.mp3\" type mpegvideo alias mp3";
			wstring stemp = wstring(s.begin(), s.end());
			LPCWSTR sw = stemp.c_str();
			mciSendString(sw, NULL, 0, NULL);

			::ShowWindow(::GetConsoleWindow(), SW_SHOW); //Shows the black box
			cout << "Give me the Value for Gauss: ";
			cin >> inputGauss;
			inputVoltage = doubleToStr(4, (inputGauss + 9.0991) / 260.65);
			inputCurrent = "4";
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