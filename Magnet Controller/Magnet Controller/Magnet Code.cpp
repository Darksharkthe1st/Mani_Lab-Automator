#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <fstream>
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
//New Commands
double calibrateTo(double Gauss);
void changeVolts(double inputVolts, POINT textPosition);
double runAndCollect(POINT runPos, string filename);
void clickAt(POINT p);
double zeroVoltage();




int delay = 0;

/*VARIABLES FOR CHANGING*/
//Minimum 6 seconds, otherwise the program will break.
int interval = 10000; //Number of milliseconds (in thousandths of seconds) per interval
//MULTIPLY THE AMOUNT OF SECONDS BY 1000
string zeroCurrent = "4"; //Current at zero gauss
const string current = "8";
string labViewPath;
string kepcoPath;

int main()
{
start:
system(filename); //explorer labview, explorer kepco
//Uncomment if current needs to be changed
/*string magVal2;
cout << "Give me the Value for Current: ";*/

SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
{
string s = "Kepco Control Panel";
wstring wstr = wstring(s.begin(), s.end());
LPCWSTR lstr = wstr.c_str();
HWND handle = FindWindow(NULL, lstr);
RECT rect;
GetWindowRect(handle, &rect);
MoveWindow(handle, 600, 0, 700, 700, false);
ShowWindow(handle, SW_MINIMIZE);

Sleep(500);
ShowWindow(handle, SW_RESTORE);
ShowWindow(handle, SW_HIDE);
ShowWindow(handle, SW_SHOW);
}
//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
//std::cout << "Hello World!\n";

if ()
while (true) {

double inputVoltage;
double zeroVoltage;
double inputGauss;
cout << "Give me the Value for Gauss: ";
string answer;
cout << "What value do you want the magnet to go to?\n";
do {
cout << "Value: ";
cin >> answer;
try {
inputGauss = stod(answer);
}
catch (exception e) {
cout << "Invalid number.\n";
continue;
}
break;
} while (true);
inputVoltage = calibrateTo(inputGauss);
zeroVoltage = zeroVoltage();

cout << "Voltage: " << inputVoltage;
cout << "; Current: " << current;

double currentVolts = inputVoltage;

cout << "CLICK F12 when you're ready to record.";


Sleep(50);
//delay += countSleep(50);
if (GetAsyncKeyState(VK_F12)) {
::ShowWindow(::GetConsoleWindow(), SW_HIDE); //Hides the black box
for (int e = 0; e < 6; e++) {
terminateIfEsc();
system("CLS");
cout << "Off:\n";
cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
cout << "Voltage: " << zeroVoltage << "; Current: 4\n";
cout << "On:\n";
cout << "Gauss: " << doubleToStr(4, inputGauss) << "\n";
cout << "Voltage: " << inputVoltage << "; Current: 4\n";
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

double calibrateTo(double Gauss) {
POINT runPosition; runPosition.x = 64; runPosition.y = 64;
POINT textPosition;
SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
cout << "Put your mouse in the text box under Vset and click F12.";
getPosOnClick(&textPosition);
cout << textPosition.x << ", " << textPosition.y << "\n";
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
changeVolts((top + bottom) / 2, textPosition);
voltage = (top + bottom) / 2;
countSleep(2000);
terminateIfEsc();
value = runAndCollect(runPosition, filename);
countSleep(500);
if (value < Gauss) bottom = (top + bottom) / 2;
else top = (top + bottom) / 2;
std::cout << value << ", " << top << ", " << bottom << "\n";
terminateIfEsc();
} while (abs(value - Gauss) > 1 && (abs(top - bottom) > 0.001));
}
POINT p;
ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED);
system("CLS");
if (abs(value - Gauss <= 1)) {
cout << "DONE!\n";
cout << "The voltage used was " << voltage << " Volts.\n";
cout << "The current used was " << current << " Amps.\n";
cout << "Gauss achieved: " << value << " Gauss. \n";
}
else {
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




//Changes the voltage of the magnet to inputVolts, current is always 4.
void changeVolts(double inputVolts, POINT textPosition) {
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
countSleep(sleepTime);
//Types in the voltage
sendString(magVal1);
terminateIfEsc();
countSleep(sleepTime);
//Goes to the current box
clickKey('\t');

//Deletes the previous text
keyDown(VK_CONTROL);
clickKey('a');
keyUp(VK_CONTROL); //Ctrl + a
keyDown(VK_DELETE); //keyUp(VK_DELETE); //Delete
terminateIfEsc();
countSleep(sleepTime);
//Types in the current
sendString(magVal2);

//Goes to and hits the magnet switch button
clickKey('\t');
keyTap(VK_RETURN);
terminateIfEsc();
countSleep(sleepTime);


clickAt(pointy);

terminateIfEsc();
countSleep(sleepTime);
countSleep(sleepTime);
terminateIfEsc();


}


//Hits run in labview and collects the data from the output .txt file
double runAndCollect(POINT runPos, string filename) {
//Clicks the run button in LabView
clickAt(runPos);
//Wait so the program has time to collect
terminateIfEsc();
countSleep(500); //TODO: ADJUST THIS TIME
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

double zeroVoltage() {
POINT runPosition; runPosition.x = 64; runPosition.y = 64;
POINT textPosition;
SetWindowPos(GetConsoleWindow(), 0, 800, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
cout << "Put your mouse in the text box under Vset and click F12.";
getPosOnClick(&textPosition);
cout << textPosition.x << ", " << textPosition.y << "\n";
string filename = "C:\\Users\\Mani Lab\\Desktop\\Calibrator Text File\\calibrator text.txt";
//double value = runAndCollect(runPosition, filename);
double value = 0;
boolean run = true;
double minimumVoltage = 0;
if (run) {

changeVolts(0, textPosition);
double minDiff = abs(runAndCollect(runPosition, filename));
int minVolts = 0;
for (int x = -3; x <= 3; x++) {
system("CLS");
cout << "PRESS ESC TO STOP\n";
cout << "DESIRED VALUE: " << 0 << "\n";
terminateIfEsc();
changeVolts(x, textPosition);
countSleep(2000);
terminateIfEsc();
value = runAndCollect(runPosition, filename);
if (abs(value) < minDiff) {
minDiff = abs(value);
minVolts = x;
}
countSleep(500);
terminateIfEsc();
}

double minimumVoltage = minVolts;

for (int digits = 1; digits <= 3; digits++) {
for (int x = -10; x <= 10; x++) {
system("CLS");
cout << "PRESS ESC TO STOP\n";
cout << "DESIRED VALUE: " << 0 << "\n";
terminateIfEsc();
changeVolts(minVolts + pow(10,digits) * x, textPosition);
countSleep(2000);
terminateIfEsc();
value = runAndCollect(runPosition, filename);
if (abs(value) < minDiff) {
minDiff = abs(value);
minimumVoltage = minVolts + pow(10, digits) * x;
}
countSleep(500);
terminateIfEsc();
}
}
}
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