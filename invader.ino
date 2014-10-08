//   Invader display - Andy Grace (c) 2014
//   Nothing much to this hardware - let's use an Arduino but 
//   AVR, ARM microcontrollers all work.  This is a simple testbed for a future commercial product, so no GPL but enjoy for now.
//
//   One 8x8 RED-GREEN common anode matrix display 1088BHG or equivalent = 128 LEDs cost: $1 each
//
//   Three '595 8-bit shift registers; 
//   One '595 driving the rows (anodes) of the display via a UDN2981A 8 x Darlington transistor array (or use 8 x PNP transistors)
//   Other two '595s driving the columns multiplexed with 8:1 - 12.5% duty cycle via current limiting resistors.
//   No PWM needed for this but other code on this hardware does shades of colour. 

// Software constants you can muck around with

const int NUMBER_ROWS          = 8;
const int NUMBER_COLUMNS       = 8;
const int RED                  = 1;
const int GREEN                = 2;
const int ORANGE               = 3;
const int END_OF_COLOURS       = 4;
const int BLAST                = 3;
const int BLAST2               = 4;
const int INITIAL_DELAY        = 36;
const int FASTEST_DELAY        = 5;
const int DELAY_STEP           = 5;
const int RANDOM_EXPLODE_VALUE = 12;

// Constants based on your hardware

const int serialDataPin        = 12;      // You choose which pins to clock and latch data trhough 
const int clockPin             = 13;
const int latchPin             = 9;

// Bitmaps of the display characters
unsigned char animations[5][8] = 
{
    {   0,   0,   0,   0,   0,   0,   0,   0 },      // blank
    {  24,  60, 126, 219, 255,  36,  90, 165 },      // invader move 1
    {  24,  60, 126, 219, 255,  36,  66,  36 },      // invader move 2
    { 129,  66, 102,   0,   0, 102,  66, 129 },      // invader explode 1
    { 129,  66,   0,  24,  24,   0,  66, 129 }       // invader explode 2
};


void setup() 
{     
    pinMode( serialDataPin,   OUTPUT );
    pinMode( clockPin,        OUTPUT );
    pinMode( latchPin,        OUTPUT );
}


void loop() {  
       
   for (int delay = INITIAL_DELAY; delay > FASTEST_DELAY; delay = delay - DELAY_STEP)   
   {
       
       int colour = RED;
       while(colour < END_OF_COLOURS)
       {
               
         int shift = 0;
         while (shift <= NUMBER_COLUMNS) 
         {
           
              int colourIndex = 1;
              int countTillExplode = random(1, RANDOM_EXPLODE_VALUE);
              
              while ( colourIndex < 3) 
              {
                   
                    if (countTillExplode == 1) 
                    {
                          // ie short blow up animation ... 
                          displayPixels(BLAST,  15, colour, shift);
                          displayPixels(BLAST2, 15, colour, shift);
                          
                          // next colour or start with RED again ... 
                          colour++;
                          if (colour == END_OF_COLOURS)  
                          {   
                              colour = RED; 
                          }
                          
                          // put invader back in middle of display then immediately exit loop
                          colourIndex = 1;
                          shift       = 0;
                          break;
                    }
                    
                   displayPixels(colourIndex, delay, colour, shift);
                   colourIndex++;
                   
              } 
         
              shift++;   
              
          } 
          
         colour++; 
         
        }
        
    }
    
}
 

void displayPixels(int character, int delay, int colour, int shift) {
  
int redPixels;
int greenPixels;
  
for (int i=0; i< delay; i++) {
    
    for (int row=0; row < NUMBER_ROWS; row++) {
      
        shiftOut(serialDataPin, clockPin, MSBFIRST, B00000001 << row);     // set up the current anode 
        
        switch (colour) 
        {
            case RED: 
               redPixels      = animations[character][row];
               greenPixels    = 0;
               break;
               
           case GREEN:
               greenPixels    = animations[character][row];
               redPixels      = 0;
               break; 
               
           case ORANGE: 
               redPixels      = animations[character][row];
               greenPixels    = animations[character][row];
               break;
        }
        
        
       shiftOut(serialDataPin, clockPin, MSBFIRST, (255 - (redPixels   << shift)));   // set the red column
       shiftOut(serialDataPin, clockPin, MSBFIRST, (255 - (greenPixels << shift)));   // set the green column
       
       digitalWrite(latchPin, HIGH);      // latch in the current 
       digitalWrite(latchPin, LOW);
       
       delayMicroseconds(10);
        
       
       //   blank the display to stop ghosting
    
       shiftOut(serialDataPin, clockPin, MSBFIRST, B00000001 << row);
       shiftOut(serialDataPin, clockPin, MSBFIRST, 255);                // all segments off 
       shiftOut(serialDataPin, clockPin, MSBFIRST, 255);
       digitalWrite(latchPin, HIGH);
       digitalWrite(latchPin, LOW);
      
     }
    
  }
  
}     
