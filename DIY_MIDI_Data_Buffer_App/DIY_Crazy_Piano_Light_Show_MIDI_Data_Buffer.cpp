/* DIY_Learning_Software_App_NoGUI_v0.cpp : This file contains the 'main' function.Program execution begins and ends there.
	See https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/ for more information on the MIDI API
*/

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <string>
#pragma comment(lib, "winmm.lib") //include the Microsoft MIDI library so we can have this applicaiton talk to the MIDI devices seen by the PC. 
using namespace std;
HMIDIIN hMidi_In_Device = NULL;;
HMIDIOUT hMidi_Out_Device = NULL;;
WCHAR Note[2];

//For the purpose of this function, see userConsoleGetDeviceNum
bool tryParse(std::string& input, int& output) {
	try {
		output = std::stoi(input); //did the user enter a integer?
	}
	catch (std::invalid_argument) { //if they did not, return false. 
		return false;
	}
	return true;
}

//This function gets user input from the console to select which MIDI device to use.  This is needed since your PC may have more than 1!
int userConsoleGetDeviceNum(int max_device) {

	std::string input;
	int x;
	// prompt the user for input
	cout << "Select MIDI Device (0, 1, 2...): ";
	// get input
	getline(std::cin, input);

	while (true)
	{
		if (!tryParse(input, x)) { //Checks if user did not enter a integer value
			if (input.compare("q") == 0) { //If user did not enter a integer value, was it a quit ("q") command?
				return -1; //If so, quit out...
			}
			//prompt user to try again
			std::cout << "Invalid MIDI Device\n";
			cout << "Select MIDI Device (0, 1, 2...): ";
			getline(std::cin, input);
		}
		else if (x > max_device || x < 0) { //Checks if user tried to enter a MIDI device that does not exist. 
			//prompt user to try again
			std::cout << "Invalid MIDI Device\n";
			cout << "Select MIDI Device (0, 1, 2...): ";
			getline(std::cin, input);
		}
		else {
			return x; //returns the MIDI device ID number that the user wants to use
		}
	}
}

/*Callback used by the MIDI-in functions. The application calls this function when: 
* Ref: https://learn.microsoft.com/en-us/windows/win32/multimedia/
 1) midiInOpen() is called, this function will print that the midi device is open
 2) midiInClos() is called, this function will print that the midi device is closed
 3) when MIDI data is received, this function will delay the data by 6ms (to solve the MIDI LED light strip issue seen here https://github.com/FastLED/FastLED/wiki/Interrupt-problems)
    This delay is needed since every time the Arduino goes to drive the LED strip, we cannot send data to the Arduino serial, otherwise that data gets corrupted.
	Once the delay if finished, the function sends the MIDI-IN data to the MIDI-out (Arduino)
	Per the MIDI 1.0 spec, the command 0x90 is for "Note On" (and "Note Off" but only when velocity is 0)
	note_val is the piano note/key
	velocity is how hard the key was pressed. 
	dwParam1 = dwMidiMessage
	dwParam2 = dwTimestamp
	For dwMidiMessage:
	High word	High-order byte	Not used.
	Low-order byte	Contains a second byte of MIDI data (when needed).
	Low word	High-order byte	Contains the first byte of MIDI data (when needed).
	Low-order byte	Contains the MIDI status.
4) The MIM_LONGERROR message is sent to a MIDI input callback function when an invalid or incomplete MIDI system-exclusive message is received.
5) The MIM_ERROR message is sent to a MIDI input callback function when an invalid MIDI message is received.
6) The MIM_MOREDATA message is sent to a MIDI input callback function when a MIDI message is received by a MIDI input device but the application is not processing MIM_DATA messages fast enough to keep up with the input device driver. The callback function receives this message only when the application specifies MIDI_IO_STATUS in the call to the midiInOpen function.

*/
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (wMsg) {
	case MIM_OPEN:
		printf("==Midi-IN Device Opened! ==\n");
		break;
	case MIM_CLOSE:
		printf("==Midi-IN Device Closed! ==\n");
		break;
	case MIM_DATA:
		if ((dwParam1 & 0x000000FF) == 0x90 || (dwParam1 & 0x000000FF) == 0x80) { //Is message a Note On or Note Off
			Sleep(6); //Critical! DO NOT REMOVE, DO NOT CHANGE!
			midiOutShortMsg(hMidi_Out_Device, dwParam1); //Send MIDI-IN to MIDI-Out
			//Print MIDI dwMidiMessage for debugging purposes
			int note_val = 0x000000FF & (dwParam1 >> 8);
			if ((dwParam1 & 0x000000FF) == 0x80) { //Per the MIDI 1.0 Spec the command 0x80 is a Note Off command
				printf("NoteOff, Note=0x%x\n", note_val);
			}
			else {
				int velocity = 0x000000FF & (dwParam1 >> 16);
				if (velocity == 0) { //Per the MIDI 1.0 Spec the command 0x90 with velocity 0 is also a Note Off command
					printf("NoteOff, Note=0x%x\n", note_val);
				}
				else {
					printf("NoteOn,  Note=0x%x, Velocity=%d\n", note_val, velocity);
				}
			}
		}
		break;
	case MIM_LONGDATA:
		printf("wMsg=MIM_LONGDATA\n");
		break;
	case MIM_ERROR:
		printf("wMsg=MIM_ERROR\n");
		break;
	case MIM_LONGERROR:
		printf("wMsg=MIM_LONGERROR\n");
		break;
	case MIM_MOREDATA:
		printf("wMsg=MIM_MOREDATA\n");
		break;
	default:
		printf("wMsg = unknown\n");
		break;
	}
	return;
}
/*Callback used by the MIDI-out functions. The application calls this function when:
* * Ref: https://learn.microsoft.com/en-us/windows/win32/multimedia/
 1) midiOutOpen() is called, this function will print that the midi device is open
 2) midiOutClos() is called, this function will print that the midi device is closed
*/
void CALLBACK MidiOutProc(
	HMIDIOUT  hmo,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
) {
	switch (wMsg) {
	case MOM_OPEN:
		printf("== Midi-OUT Device Opened! ==\n");
		break;
	case MOM_CLOSE:
		printf("== Midi-OUT Device Closed! ==\n");
		break;
	default:
 		printf("wMsg = unknown\n");
		break;
	}
	return;
}

//This function opens and connects this application to the user selected MIDI input device 
void OpenMidi_Input_Device()
{
	UINT Midi_IN_DeviceNum = -1;
	int Selected_Midi_IN_Device = -1;
	MMRESULT rv;
	MIDIINCAPS caps;

	//Detects how many MIDI Input devices are on the PC
	Midi_IN_DeviceNum = midiInGetNumDevs(); 
	if (Midi_IN_DeviceNum == 0) {
		fprintf(stderr, "No MIDI-IN Devices Found...");
		return;
	}

	//Prints the names of those MIDI Input devices to the user
	printf("== Detected Midi-IN Devices == \n");
	for (unsigned int i = 0; i < Midi_IN_DeviceNum; ++i) {
		midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
		printf("%d : name = ", i);
		std::wcout << caps.szPname <<std::endl;
		printf("== Opening Midi-IN Device == \n");
	}
	//Asks user what MIDI input device to use.  This should be the MIDI devce from the piano keyboard. 
	Selected_Midi_IN_Device = userConsoleGetDeviceNum(Midi_IN_DeviceNum-1);
	if (Selected_Midi_IN_Device == -1) {
		fprintf(stderr, "midiInOpen() failed...No Device Selected\n");
		return;
	}
	//Open MIDI Device selected by the user.
	rv = midiInOpen(&hMidi_In_Device, Selected_Midi_IN_Device, (DWORD_PTR)(void*)MidiInProc, 0, CALLBACK_FUNCTION);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiInOpen() failed...rv=%d", rv);
		return;
	}

}
//This function opens and connects this application to the user selected MIDI output device
void OpenMidi_Output_Device()
{
	UINT Midi_OUT_DeviceNum;
	INT Selected_Midi_Out_Device = -1;
	MMRESULT rv;
	MIDIOUTCAPS caps;

	//Detects how many MIDI Output devices are on the PC
	Midi_OUT_DeviceNum = midiOutGetNumDevs();
	if (Midi_OUT_DeviceNum == 0) {
		fprintf(stderr, "No MIDI-OUT Devices Found...");
		return;
	}

	//Prints the names of those MIDI Output devices to the user
	printf("== Detected Midi-OUT Devices == \n");
	for (unsigned int i = 0; i < Midi_OUT_DeviceNum; ++i) {
		midiOutGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
		printf("%d : name = ", i);
		std::wcout << caps.szPname << std::endl;
	}

	//Asks user what MIDI output device to use.  This should be the MIDI device from the Arduino. 
	Selected_Midi_Out_Device = userConsoleGetDeviceNum(Midi_OUT_DeviceNum - 1);
	if (Selected_Midi_Out_Device == -1) {
		fprintf(stderr, "midiOutOpen() failed...No Device Selected\n");
		return;
	}

	//Open MIDI Device selected by the user.
	printf("== Opening Midi-OUT Device == \n");
	rv = midiOutOpen(&hMidi_Out_Device, Selected_Midi_Out_Device, (DWORD_PTR)(void*)MidiOutProc, 0, CALLBACK_FUNCTION);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiOutOpen() failed...rv=%d", rv);
		return;
	}
}

//Closes and Stops MIDI devices
void CloseMidi_Input_Device() {
	
	MMRESULT rv;
	printf("== Closing Midi-IN Device == \n");
	rv = midiInStop(hMidi_In_Device);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiInStop() failed...rv=%d", rv);
		return;
	}

	rv = midiInClose(hMidi_In_Device);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiInClose() failed...rv=%d", rv);
		return;
	}
}
void CloseMidi_Output_Device() {

	MMRESULT rv;
	printf("== Closing Midi-OUT Device == \n");
	rv = midiOutClose(hMidi_Out_Device);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiOutClose() failed...rv=%d", rv);
		return;
	}
}



int main(int argc, char* argv[])
{
	printf("== Started:Crazy Piano Light Show - Pass Through Software (press q to quit) == \n");
	OpenMidi_Input_Device(); //This function connects this Application to your MIDI IN Device
	OpenMidi_Output_Device(); //This function connects this Application to your MIDI Out Device
	midiInStart(hMidi_In_Device); //The midiInStart function starts MIDI input on the specified MIDI input device. 
	printf("== Start Playing! (press q to quit) == \n");
	while (true) {

		if (!_kbhit()) { //if user provides console input...
			Sleep(100);
			continue;
		}
		int c = _getch(); //read console..
		if (c == VK_ESCAPE) break; //if q or VK_escape, stop appllication
		if (c == 'q') break;
		
		
	}

	//Clean up
	CloseMidi_Input_Device();
	CloseMidi_Output_Device();
	hMidi_In_Device = NULL;
	hMidi_Out_Device = NULL;
}