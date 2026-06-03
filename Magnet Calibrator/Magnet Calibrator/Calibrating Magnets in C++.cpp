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
const string current = "4.2";
POINT textPosition;

/*VARIABLES FOR CHANGING*/
//Minimum 6 seconds, otherwise the program will break.
int interval = 10000; //Number of milliseconds (in thousandths of seconds) per interval
//MULTIPLY THE AMOUNT OF SECONDS BY 1000
string zeroVoltage = "0.071"; //Voltage at zero gauss

string zeroCurrent = "4.2"; //Current at zero gauss

int main() {
	textPosition.x = 700; textPosition.y = 270;

	//Open all necessary applicaitons for magnet control, move the terminal to a predictable spot on the screen
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

	//Move the cursor to a position on the screen
	{
		POINT stopPosition; stopPosition.x = 110; stopPosition.y = 70;
		POINT runPosition; runPosition.x = 87; runPosition.y = 65;
		clickAt(stopPosition);
		Sleep(500);
		clickAt(runPosition);
	}

	//showMousePos();

	//Wait for user input
	system("Pause");
	boolean run = true;

	//Run until stopped:
	while (run) {
		POINT runPosition; runPosition.x = 64; runPosition.y = 64;
		POINT textPosition;
		string answer;
		double setPoint = 0;

		//Loop until user provides appropriate input (type check until integer found)
		while (true) {
			cout << "What value do you want the magnet to go to?\n";
			do {
				cout << "Value: ";
				cin >> answer;
				try {
					setPoint = stod(answer);
				}
				catch (exception e) {
					cout << "Invalid number.\n";
					continue;
				}
				break;
			} while (true);

			//Calibrate the magnet (see function)
			if (abs(setPoint) <= 1) {
				calibToZero();
			}
			else {
				calibrateTo(setPoint);
			}
			//Return control back to user once calibration is complete
			cout << "DONE!";
			system("Pause");
			system("CLS");
		}

	}
	//showMousePos();
	// double zeroG = calibToZero();
	// system("Pause");
	// double ThreeThousGauss = calibrateTo(3000);
	// cout << ":)";
}

//Testing function to show the current mouse position in the terminal
void showMousePos() {
	POINT mousePos;
	while (true) {
		if (GetCursorPos(&mousePos)) {
			cout << mousePos.x << ", " << mousePos.y << "\n";
		}
	}
}

//Find the voltage that sets the magnet to zero via checking values
double calibToZero() {
	//Move the console window out of the way of the calibration program
	SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	cout << "Put your mouse in the text box under Vset and click F12.";
	cout << textPosition.x << ", " << textPosition.y << "\n";
	string filename = "C:\\Users\\Mani Lab\\Desktop\\Calibrator Text File\\calibrator text.txt";
	//double value = runAndCollect(runPosition, filename);
	double value = 0;
	boolean run = true;
	double minimumVoltage = 0;
	
	if (run) {

		//Sets the voltage in the machine to zero via Kepco
		changeVolts(0);
		//Collects data about the desired magnet values (in gauss) from the file provided
		double minDiff = abs(runAndCollect(filename));
		double minVolts = 0;

		//Calibrate below and above the value (zero in this case)
		for (int x = -2; x <= 2; x++) {
			//Clear the screen
			system("CLS");

			//Change the value
			cout << "PRESS ESC TO STOP\n";
			cout << "DESIRED VALUE: " << 0 << "\n";
			
			//End the program if the escape key is held down (emergency shutdown)
			terminateIfEsc();

			//Delay to avoid overloading the machine
			Sleep(1000);

			//Change the voltage of the magnet using Kepco as desired
			changeVolts(x * 0.1);
			//Emergency shutdown again
			terminateIfEsc();
			//Get data from separate serial comm reader script stored in file
			value = runAndCollect(filename);
			Sleep(1000);
			//If we're closer to zero than we were before, save the voltage that got us closes
			if (abs(value) < minDiff) {
				minDiff = abs(value);
				minVolts = x * 0.1;
			}
			//Emergency shutdown again
			terminateIfEsc();
		}

		
		double minimumVoltage = minVolts;
		double prevMin = minVolts;
		//Calibrate with 2 decimal places of accuracy first, then three
		for (int digits = 2; digits <= 3; digits++) {
			//Check 10 under and 10 above the heuristic minimum to get a more specific minimum value (so if it's .1, we check .01, ... , .09, .11, ..., .19).
			for (int x = -10; x <= 10; x++) {
				//Clear screen, print values for user
				system("CLS");
				cout << "PRESS ESC TO STOP\n";
				cout << "DESIRED VALUE: " << 0 << "\n";
				terminateIfEsc();

				//Change magnet voltage via kepco, sleep to avoid overload, emergency esc
				changeVolts(prevMin + pow(0.1, digits) * x);
				Sleep(1000);
				terminateIfEsc();

				//Get calibrated value from file (information from gaussmeter), print for user 
				value = runAndCollect(filename);
				cout << "VALUE: " << value << "; VOLTS: " << prevMin + pow(0.1, digits) * x;

				//Save value if more accurate, return if as accurate as can be
				if (abs(value) < minDiff) {
					minDiff = abs(value);
					minimumVoltage = prevMin + pow(0.1, digits) * x;

					if (abs(value) < 1) return minimumVoltage;
				}
				Sleep(1000);
				terminateIfEsc();
			}
			prevMin = minimumVoltage;
		}
	}

	//Return window control back to user, clear screen, present them the result of the calibration
	POINT p;
	ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED);
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

//Calibrates the magnet to the desired gauss value via Kepco and the gaussmeter reader script
double calibrateTo(double Gauss) {
	//Move the window out of the way, track the file where voltage is stored
	SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	string filename = "C:\\Users\\Mani Lab\\Desktop\\Calibrator Text File\\calibrator text.txt";
	//double value = runAndCollect(runPosition, filename);
	//constants controlling how specific calibration will be
	double top = 30;
	double bottom = -30;
	double value = 0;
	boolean run = true;
	double voltage = 0;
	POINT dummy;
	system("Pause");
	//clickAt(runPosition);
	
	if (run) {
		do {
			//Clear screen, print values, emergency esc if needed
			system("CLS");
			cout << "PRESS ESC TO STOP\n";
			cout << "DESIRED VALUE: " << Gauss << "\n";
			terminateIfEsc();

			//Change voltage via kepco, save new voltage, sleep to avoid machine overload, emergency esc
			changeVolts((top + bottom) / 2);
			voltage = (top + bottom) / 2;
			Sleep(3000);
			terminateIfEsc();

			//Get the stored value from the gaussmeter script, print it out for the user
			value = runAndCollect(filename);
			cout << "CURRENT VALUE: " << value << "; VOLTS: " << voltage << "\n";

			//Using binary search, go up if value too low, go down if value too high
			if (value < Gauss) bottom = (top + bottom) / 2;
			else top = (top + bottom) / 2;

			//Print values, emergency esc if needed
			std::cout << value << ", " << top << ", " << bottom << "\n";
			terminateIfEsc();
		} while (abs(value - Gauss) > 0.1 && (abs(top - bottom) > 0.001));
	}

	//Clear screen, calculate values
	system("CLS");
	double minDiff = abs(value - Gauss);
	double minimumVoltage = voltage;
	double prevMin = voltage;

	//If we're close enough, stop the calibration
	if (abs(value - Gauss <= 1)) {
		cout << "DONE!\n";
		cout << "The voltage used was " << voltage << " Volts.\n";
		cout << "The current used was " << current << " Amps.\n";
		cout << "Gauss achieved: " << value << " Gauss. \n";
	}
	else { //Otherwise, calibrate for a more specfic value
		//First loop on degree of specificity (2 decimal places, 3 decimal places)
		for (int digits = 2; digits <= 3; digits++) {
			//Next, loop from 10 above to 10 below (so for .1 it's .01, ..., .09, .11, ..., .19
			for (int x = -10; x <= 10; x++) {
				//Clear screen, print info, emergency esc if needed
				system("CLS");
				cout << "PRESS ESC TO STOP\n";
				cout << "DESIRED VALUE: " << 0 << "\n";
				terminateIfEsc();

				//Change the voltage of the magnet, sleep for Kepco, emergency esc if needed
				changeVolts(prevMin + pow(0.1, digits) * x);
				Sleep(1000);
				terminateIfEsc();

				//Check gaussmeter output in file, save if good, end function if good enough
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

		//Print user information about how specific the gauss value was able to get
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

//Helper function--used for testing to find the right mouse position for manual control
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
	//Usual pointer spot for changing voltage
	POINT pointy;
	pointy.x = 700; pointy.y = 400;
	//Click the text box
	clickAt(pointy);

	//Setup variables to store information that will be soon inputted
	int sleepTime = 50;
	string inputVoltage = doubleToStr(4, inputVolts);
	std::cout << "Voltage: " << inputVoltage;
	std::cout << "; Current: " << current << "\n";
	string magVal1 = inputVoltage;
	string magVal2 = current;

	//Click the text box
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

	//terminate if needed, sleep to avoid overload, terminate if needed
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

//Clicks the mouse at a point using the windows.h library
void clickAt(POINT p) {
	SetCursorPos(p.x, p.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

//Clicks a keyboard key using the windows.h library
void clickKey(char c) {

	//Make an 'input' for the library, fill it with character data, send it
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = 0;
	Input.ki.wVk = VkKeyScanA(c); //Character to output
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));

	//Release the key so the input actually gets sent instead of being held down (like holding down c on your keyboard without letting go as opposed to tapping c)
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

//Press a key down
void keyDown(short s) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = s; //Does the command connected to the short
	Input.ki.dwFlags = 0; //Key Pressed down
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

//Let a key go
void keyUp(short s) {
	INPUT Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = s; //Does the command connected to the short
	Input.ki.dwFlags = KEYEVENTF_KEYUP; //Key Pressed down
	SendInput(1, &Input, sizeof(Input));
	ZeroMemory(&Input, sizeof(Input));
}

//Press a key, let go of a key
void keyTap(short s) {
	keyUp(s); keyDown(s);
}

//Type a string (delayed to avoid system overload)
void sendString(string s) {
	for (int i = 0; i < s.length(); i++) {
		Sleep(10);
		clickKey(s[i]);
	}
	//return s.length() * 10;
}

//counts a global variable delay to track all thread.sleep commands
//This is used above for 10 second periods--7000ms delay means add 3000ms to get to 10000ms
void countSleep(DWORD dwMilliseconds) {
	terminateIfEsc();
	Sleep(dwMilliseconds);
	delay += dwMilliseconds;
}

//Convert a double value to a string for typing
string doubleToStr(int decimalPlaces, double d) {
	d = round(d * 10000) / 10000;
	string str = to_string(d);
	while (str[str.length() - 1] == '0') {
		str = str.substr(0, str.length() - 1);
	}
	str = str + "0";
	return str;
}

//Terminate the program if the escape key is currently being held down
void terminateIfEsc() {
	if (GetAsyncKeyState(VK_ESCAPE)) {
		//Show the console to the user, end the program FORCEFULLY
		::ShowWindow(::GetConsoleWindow(), SW_SHOW);
		terminate();
	}
}
