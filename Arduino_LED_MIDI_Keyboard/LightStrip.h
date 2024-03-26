


#define LED_PIN_DATA 10
#define NUM_LEDS 144
#define QUARTER_NOTE 350 //black fill w/ staff
#define HALF_NOTE 700    //hole w/ staff
#define WHOLE_NOTE 1400 //hole w/o staff
#define T_WHOLE_NOTE 2800 //Tied Hole note





typedef struct notes_obj{
  char note;
  int duration = WHOLE_NOTE;
  int octive = 0;  
  int duration_counter = 0;
  bool is_active = false;
}NOTES_OBJ;

void noteOn(int noteByte, int velocityByte, int color_select_val); 
void noteOff(int noteByte);
void quick_init();
void showLED();


