#include <MIDI.h>
#include "LightStrip.h"
#include <Keypad.h>
#include <Key.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// -----------------------------------------------------------------------------

// This example shows the old way of checking for input messages.
// It's simpler to use the callbacks now, check out the dedicated example.

#define LED 13                   // LED pin on Arduino Uno used for debugging.  Turns on if it detects MIDI NoteOn commands and off when it detects MIDI NoteOff commands.
byte noteByte;
byte velocityByte;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {{'D', 'C', 'B', 'A'},
                             {'#', '9', '6', '3'},
                             {'0', '8', '5', '2'},
                             {'*', '7', '4', '1'}};

byte colPins[COLS] = {5, 4, 3, 2};
byte rowPins[ROWS] = {9, 8, 7, 6};                           
Keypad customKeypad = Keypad(makeKeymap(hexaKeys),  rowPins, colPins, ROWS, COLS);   
int  color_select_val=0;
char user_in_clr_sel;
// -----------------------------------------------------------------------------

void BlinkLed(byte num)         // Basic blink function
{
    for (byte i=0;i<num;i++)
    {
        digitalWrite(LED,HIGH);
        delay(50);
        digitalWrite(LED,LOW);
        delay(50);
    }
}

// -----------------------------------------------------------------------------

void setup()
{
    pinMode(LED, OUTPUT);
    quick_init(); 
    noteOn(60, 50, 0);      //test LED strip
    delay(400);
    noteOff(60);            //test LED strip
    MIDI.begin();           // Launch MIDI, by default listening to channel 1.

   //This waits for user to input a number (just one number) on the keypad to select a color for the strip LEDs.  See LightStrip.ccp for what color corresponds to what number on the keypad
   while(1){
      user_in_clr_sel = customKeypad.getKey(); //read input from keypad
      //if value from keypad is not a number (0-9) try again
      if(user_in_clr_sel && user_in_clr_sel!='#' && user_in_clr_sel!='A' && user_in_clr_sel!='B' && user_in_clr_sel!='C' && user_in_clr_sel!='D' && user_in_clr_sel!='*'){
        color_select_val = user_in_clr_sel - '0'; //convert char to int
        break;
      }
   }
}

void loop()
{
  int count = 0;
    if (MIDI.read())                // Is there a MIDI message incoming ?
    {
        switch(MIDI.getType())      // Get the type of the message we caught
        {
            case midi::ProgramChange:       // If it is a Program Change,
                BlinkLed(MIDI.getData1());  // blink the LED a number of times
                                            // correponding to the program number
                                            // (0 to 127, it can last a while..)
                break;
            // See the online reference for other message types
            case midi::NoteOn:                 // If it is a NoteOn,
              noteByte = MIDI.getData1();      // Get Note Byte (played note)
              velocityByte = MIDI.getData2();  // Get Veloicty Byte (how hard the note was pressed/played)
              if ( velocityByte > 0){          // If Veolicty is greater than 0
                digitalWrite(13,HIGH);//turn on Arduino built-in led for debug testing
                noteOn(noteByte, velocityByte, color_select_val); //turn on LED strip light corresponding to note played
              }else{
                //THIS MIGHT NOT BE NEEDED.  THE MIDI Lib might be smart enough to handle this...however if it doesnt then: 
                noteOff(noteByte); //Per the MIDI spec, if the command is "NoteOn" (0x90) but velocity is 0, treat this as a NoteOff.  This function turns off the LED light on the LED light strip
                digitalWrite(LED,LOW);//turn off Arduino built-in led for debug testing
              }      
              break;
            case midi::NoteOff:// If it is a NoteOFF,
              digitalWrite(LED,LOW);//turn off Arduino built-in led for debug testing
              noteByte = MIDI.getData1(); // Get Note Byte (played note)
              noteOff(noteByte); //This function turns off the LED light on the LED light strip
              break;        
            default:
                break;
        }
    }


}
