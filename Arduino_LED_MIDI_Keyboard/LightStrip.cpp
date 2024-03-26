#include "LightStrip.h"
 
//#define FASTLED_ALLOW_INTERRUPTS 0
 #include <FastLED.h>

CRGB leds[NUM_LEDS];
void quick_init() {
  //TO BE EDITIED BY USER - For other strips, change "NEOPIXEL" to your LED strip device
  FastLED.addLeds<NEOPIXEL,LED_PIN_DATA>(leds, NUM_LEDS);
}
//Note_to_LED_LUT:  TO BE EDITED BY USER - There are 120 possible MIDI note values, this table maps a MIDI note value to one of the 144 LEDs (not all LEDs are used)
//                                         Values in this array correspond to a LED.  LEDs on the left end of the strip correspond to a higher value and lower octive.
//                                         The values in the array may need tweeked depending on keyboard layout and LED strip position.
//                                         The example numbers in here only support a 61-key keyboard piano.
                                    //C	 C#	  D	 D#	  E	  F	 F#	  G	 G#	  A	 A#	  B
int Note_to_LED_LUT[120] =       {    0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, //octive -2
                                      0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, //octive -1
                                      0,	0,	0,	0,	0,	0,	0,	0,	0,  0,	0,  0, //octive  0
                                    118,116,113,111,110,107,106,104,102,100, 98, 96, //octive  1
                                     94, 92, 90, 88, 86, 84, 82, 80, 78, 77, 75, 73, //octive  2
                                     71, 69, 67, 65, 63, 61, 59, 57, 56, 54, 52, 50, //octive  3
                                     48, 46, 44, 42, 40, 38, 36, 34, 32, 31, 28, 27, //octive  4
                                     25, 23, 21, 19, 17, 15, 13, 11,	9,  7,	5,  3, //octive  5
                                      0,	0,	0,	0,  0,	0,	0,	0,	0,  0,	0,  0, //octive  6
                                      0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0};//octive  7
                            //R   G   B      keypad value and resulting color
int RGB_Selector[10][3] = { {255,  0,  0}, //0 RED
                            {255,100,  0}, //1 Orange
                            {255,255,  0}, //2 Yellow
                            {  0,255,  0}, //3 Green
                            {125,255,255}, //4 Bright Blue
                            {255,  0,255}, //5 Pink
                            {128,  0,255}, //6 Purple
                            {  0,  0,255}, //7 Blue
                            {255,180,255}, //8 Light Pink
                            {255,255,255}};//9 While
                    
//Takes in the note value and velocity value from MIDI input as well as a color selection from the keypad.
//Lights up LED light corresponding to the MIDI note value
void noteOn(int noteByte, int velocityByte, int color_select_val){ 
  float brightness_mod = 0.5; //used to dim or brighten the LED lights corresponding to how hard (velocity) the key was pressed
  if(velocityByte >= 84){//lound = Bright LED, f or higher
    brightness_mod = 1;
  }else if(velocityByte>=64 && velocityByte<84){ //moderately loud = semi Bright LED, mf
    brightness_mod = 0.75;
  }else if(velocityByte>=44 && velocityByte<64){ //moderately soft = Dim LED, mp
    brightness_mod = 0.50;
  }else if(velocityByte>=24 && velocityByte<44){ //soft = Dimmer LED, p or lower 
    brightness_mod = 0.25;
  }else{ //very soft = Dimmest LED, pp or lower
    brightness_mod = 0.10;
  }
  leds[Note_to_LED_LUT[noteByte]] = CRGB(RGB_Selector[color_select_val][0]*brightness_mod,RGB_Selector[color_select_val][1]*brightness_mod,RGB_Selector[color_select_val][2]*brightness_mod); FastLED.show(); delay(0);       
}

//Takes in the note value from MIDI input and turns off LED corresponding to the note. 
void noteOff(int noteByte ) {
  leds[Note_to_LED_LUT[noteByte]]  = CRGB::Black; FastLED.show(); //turns LED off
}





