#include <iostream>
#include <fstream>
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
double runAndCollect(string filename);
void getPosOnClick(LPPOINT p);
void clickAt(POINT p);
void changeVolts(double inputVolts);
double calibrateTo(double Gauss);
void showMousePos();
double calibToZero();
int delay = 0;
const string current = "8";
POINT textPosition;

/*VARIABLES FOR CHANGING*/
//Minimum 6 seconds, otherwise the program will break.
int interval = 10000; //Number of milliseconds (in thousandths of seconds) per interval
//MULTIPLY THE AMOUNT OF SECONDS BY 1000
string zeroVoltage = "0.071"; //Voltage at zero gauss

string zeroCurrent = "4"; //Current at zero gauss


int main()
{
start:

	textPosition.x = 700; textPosition.y = 270;

	{
		system("explorer C:\\Program Files\\National Instruments\\LabVIEW 2017\\LabVIEW.exe");
		SetWindowPos(GetConsoleWindow(), 0, 700, 450, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		string s = "Kepco Control Panel";
		wstring wstr = wstring(s.begin(), s.end());
		LPCWSTR lstr = wstr.c_str();
		HWND handle = FindWindow(NULL, lstr);
		RECT rect;
		GetWindowRect(handle, &rect);
		MoveWindow(handle, 600, 0, 700, 450, false);
		ShowWindow(handle, SW_MINIMIZE);

		Sleep(500);
		ShowWindow(handle, SW_RESTORE);
		ShowWindow(handle, SW_HIDE);
		ShowWindow(handle, SW_SHOW);
	}
	//Uncomment if current needs to be changed
	/*string magVal2;
	cout << "Give me the Value for Current: ";
	cin >> magVal2;*/
	Sleep(1000);
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	//std::cout << "Hello World!\n";

	cout << "Interval (in seconds): ";
	cin >> interval;
	interval *= 1000;

	cout << "When you're ready, I will find voltage for 0 Gauss.\n";
	system("Pause");
	double zeroVoltage = calibToZero();
	cout << "Zero Gauss at " << zeroVoltage << "V, " << current << "A.\n";
	while (true) {
		double inputVoltage;
		double inputGauss;
		cout << "Give me the Value for Gauss: ";
		cin >> inputGauss;

		cout << "When you're ready, I will find voltage for " << inputGauss << " Gauss.\n";
		system("Pause");
		inputVoltage = calibrateTo(inputGauss);
		cout << "Voltage: " << inputVoltage;
		cout << "; Current: " << current;

		Sleep(50);
		cout << "When you're ready, click a key to start the on/off cycles.\n";
		system("Pause");
			for (int e = 0; e < 6; e++) {
				terminateIfEsc();
				system("CLS");
				cout << "Off:\n";
				cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
				cout << "Voltage: " << zeroVoltage << "; Current: " << current << "\n";
				cout << "On:\n";
				cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
				cout << "Voltage: " << inputVoltage << "; Current: " << current << "\n";
				cout << "Current State: ";
				delay = 0;
				changeVolts(inputVoltage);
				Sleep(interval - delay);
				delay = 0;
				/*Now we turn the magnet off*/
				changeVolts(zeroVoltage);
				Sleep(interval - delay);
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


void showMousePos() {
	POINT mousePos;
	while (true) {
		if (GetCursorPos(&mousePos)) {
			cout << mousePos.x << ", " << mousePos.y << "\n";
		}
	}
}


double calibToZero() {
	SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	cout << "Put your mouse in the text box under Vset and click F12.";
	cout << textPosition.x << ", " << textPosition.y << "\n";
	string filename = "C:\\Users\\Mani Lab\\Desktop\\Calibrator Text File\\calibrator text.txt";
	//double value = runAndCollect(runPosition, filename);
	double value = 0;
	boolean run = true;
	double minimumVoltage = 0;
	if (run) {

		changeVolts(0);
		Sleep(1000);
		double minDiff = abs(runAndCollect(filename));
		double minVolts = 0;
		boolean stopper = false;
		minimumVoltage = minVolts;
		double prevMin = minVolts;
		double prevDiff = minDiff;
		for (int digits = 1; digits <= 2; digits++) {
			prevDiff = 1000;
			for (int x = -5; x <= 5; x++) {
				system("CLS");
				cout << "PRESS ESC TO STOP\n";
				cout << "DESIRED VALUE: " << 0 << "\n";
				terminateIfEsc();
				changeVolts(prevMin + pow(0.1, digits) * x);
				Sleep(2000);
				terminateIfEsc();
				value = runAndCollect(filename);
				cout << "VALUE: " << value << "; VOLTS: " << prevMin + pow(0.1, digits) * x;
				
				if ((abs(value)) > prevDiff) {
					stopper = true;
				}
				if (stopper) {
					if ((abs(value)) > prevDiff) {
						break;
					}
					stopper = false;
				}
				prevDiff = abs(value);
				if (abs(value) < minDiff) {
					minDiff = abs(value);
					minimumVoltage = prevMin + pow(0.1, digits) * x;

					if (abs(value) <= 1) return minimumVoltage;
				}
				terminateIfEsc();
			}
			prevMin = minimumVoltage;
		}
	}
	POINT p;
	system("CLS");
	if (abs(value) <= 1) {
		cout << "DONE!\n";
		cout << "The voltage used was " << minimumVoltage << " Volts.\n";
		cout << "The current used was " << current << " Amps.\n";
		cout << "Gauss achieved: " << value << " Gauss. \n";
	}
	else {
		cout << "Done.\n";
		cout << "Could not get perfectly precise (limits of magnet).\n";
		cout << "The voltage used was " << minimumVoltage << " Volts.\n";
		cout << "The current used was " << current << " Amps.\n";
		cout << "Gauss achieved: " << value << " Gauss. \n";
		cout << "Difference from desired value: " << value << "\n";
	}
	system("Pause");
	//Plan is to have the program record the location when the user clicks F12,
	//Then use that position as the position of the magnet program by KEPCO.
	//I did think about using KEPCO's labview built-in functionality, but couldn't find much easy
	//documentation on how to get it set up.
	return minimumVoltage;
}

double calibrateTo(double Gauss) {
	SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	string filename = "C:\\Users\\Mani Lab\\Desktop\\Calibrator Text File\\calibrator text.txt";
	//double value = runAndCollect(runPosition, filename);
	double top = 30;
	double bottom = -30;
	double value = 0;
	boolean run = true;
	double voltage = 0;
	if (run) {
		do {
			system("CLS");
			cout << "PRESS ESC TO STOP\n";
			cout << "DESIRED VALUE: " << Gauss << "\n";
			terminateIfEsc();
			changeVolts((top + bottom) / 2);
			voltage = (top + bottom) / 2;
			Sleep(3000);
			terminateIfEsc();
			value = runAndCollect(filename);
			cout << "CURRENT VALUE: " << value << "; VOLTS: " << voltage << "\n";
			if (value < Gauss) bottom = (top + bottom) / 2;
			else top = (top + bottom) / 2;
			std::cout << value << ", " << top << ", " << bottom << "\n";
			terminateIfEsc();
		} while (abs(value - Gauss) > 0.1 && (abs(top - bottom) > 0.001));
	}
	system("CLS");

	if (abs(value - Gauss <= 1)) {
		cout << "DONE!\n";
		cout << "The voltage used was " << voltage << " Volts.\n";
		cout << "The current used was " << current << " Amps.\n";
		cout << "Gauss desired:  " << Gauss << " Gauss. \n";
		cout << "Gauss achieved: " << value << " Gauss. \n";
	}
	else {

		double minDiff = abs(value - Gauss);
		double minimumVoltage = voltage;
		double prevMin = voltage;
		for (int digits = 2; digits <= 3; digits++) {
			for (int x = -10; x <= 10; x++) {
				system("CLS");
				cout << "PRESS ESC TO STOP\n";
				cout << "DESIRED VALUE: " << 0 << "\n";
				terminateIfEsc();
				changeVolts(prevMin + pow(0.1, digits) * x);
				Sleep(1000);
				terminateIfEsc();
				value = runAndCollect(filename);
				cout << "VALUE: " << value << "; VOLTS: " << prevMin + pow(0.1, digits) * x;
				if (abs(value - Gauss) < minDiff) {
					minDiff = abs(value - Gauss);
					minimumVoltage = prevMin + pow(0.1, digits) * x;

					if (abs(value - Gauss) < 1) return minimumVoltage;
				}
				Sleep(1000);
				terminateIfEsc();
			}
			prevMin = minimumVoltage;
		}
		cout << "MINIMUM VOLTAGE: " << minimumVoltage << "\n";
		cout << "Done.\n";
		cout << "Could not get perfectly precise (limits of magnet).\n";
		cout << "The voltage used was " << voltage << " Volts.\n";
		cout << "The current used was " << current << " Amps.\n";
		cout << "Gauss achieved: " << value << " Gauss. \n";
		cout << "Difference from desired value: " << value - Gauss << "\n";
	}
	system("Pause");
	//Plan is to have the program record the location when the user clicks F12,
	//Then use that position as the position of the magnet program by KEPCO.
	//I did think about using KEPCO's labview built-in functionality, but couldn't find much easy
	//documentation on how to get it set up.
	return voltage;
}


void getPosOnClick(LPPOINT p) {
	while (true) {
		if (GetAsyncKeyState(VK_F12)) {
			break;
		}
	}
	GetCursorPos(p);
}

//Changes the voltage of the magnet to inputVolts, current is always 4.
void changeVolts(double inputVolts) {
	POINT pointy;
	pointy.x = 700; pointy.y = 400;

	clickAt(pointy);

	int sleepTime = 50;
	string inputVoltage = doubleToStr(4, inputVolts);
	std::cout << "Voltage: " << inputVoltage;
	std::cout << "; Current: " << current << "\n";
	string magVal1 = inputVoltage;
	string magVal2 = current;

	clickAt(textPosition);
	terminateIfEsc();
	//Deletes the previous text
	keyDown(VK_CONTROL);
	clickKey('a');
	keyUp(VK_CONTROL); //Ctrl + a
	keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete
	terminateIfEsc();
	Sleep(sleepTime);
	//Types in the voltage
	sendString(magVal1);
	terminateIfEsc();
	Sleep(sleepTime);
	//Goes to the current box
	clickKey('\t');

	//Deletes the previous text
	keyDown(VK_CONTROL);
	clickKey('a');
	keyUp(VK_CONTROL); //Ctrl + a
	keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete
	terminateIfEsc();
	Sleep(sleepTime);
	//Types in the current
	sendString(magVal2);

	//Goes to and hits the magnet switch button
	clickKey('\t');
	keyTap(VK_RETURN);
	terminateIfEsc();
	Sleep(sleepTime);


	clickAt(pointy);

	terminateIfEsc();
	Sleep(sleepTime);
	Sleep(sleepTime);
	terminateIfEsc();


}


//Hits run in labview and collects the data from the output .txt file
double runAndCollect(string filename) {
	//Clicks the run button in LabView
	//clickAt(runPos);
	//Wait so the program has time to collect
	terminateIfEsc();
	//Get the value from the .txt file
	ifstream input = ifstream(filename, ios::beg);
	string value;
	input >> value;
	value = value.substr(0, value.length() - 1); 
	//Convert to double and return
	return stod(value);
}


void clickAt(POINT p) {
	SetCursorPos(p.x, p.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
