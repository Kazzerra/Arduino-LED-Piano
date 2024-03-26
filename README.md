Setup hardware by following the "Arduino_Block_Diagram.PNG"

If using a 3-wire LED Strip, you must run the applicaton found in DIY_MIDI_Data_Buffer_App/x64/Release/DIY_MIDI_Data_Buffer_App.exe
When running the App, make sure the MIDI-to-USB connected is plugged into the PC, Piano, and Arduino.

When running the app you should be prompted for a few things namely, the MIDI input and MIDI output devices.  Follow the instructions displayed on the console. See Example Below:
== Started:Crazy Piano Light Show - Pass Through Software (press q to quit) ==
== Detected Midi-IN Devices ==
0 : name = UM-ONE
== Opening Midi-IN Device ==
Select MIDI Device (0, 1, 2...): 0
==Midi-IN Device Opened! ==

== Detected Midi-OUT Devices ==
0 : name = Microsoft GS Wavetable Synth
1 : name = UM-ONE
Select MIDI Device (0, 1, 2...): 1
== Opening Midi-OUT Device ==
== Midi-OUT Device Opened! ==

== Start Playing! (press q to quit) ==
NoteOn,  Note=0x4f, Velocity=46
NoteOff, Note=0x4f

For the Arduino code to comple, you must use the Arduino IDE.
You must install the following libraries:
FastLED 3.6.0 or FASTLED NeoPixel 1.0.3
Keypad 3.1.1
MIDI Library 5.0.2

NOTICE: when using the arduino UNO, you must remove the wire going into PIN 0 when programming the device (this is because the USB also uses these same serial ports as PIN 0/1.  If you have something plugged into PIN 0, then the programming may not complete).  Once programmed, removed the USB cable and reconnect the MIDI-In circuit back into PIN 0.

When everything is running, press 0-9 on the Arduino key pad to select color.  0 is red, 1 is orange, 2 is yellow, 3 is green, 4 is bright blue, 5 is pink, 6 is purple, 7 is blue, 8 is light pink, 9 is white.
