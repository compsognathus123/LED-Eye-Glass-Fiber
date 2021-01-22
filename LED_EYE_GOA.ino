#include <FastLED.h>

#define NUM_LEDS 72
#define DATA_PIN 10
#define NUM_FIBERS 36
#define POTI_PIN A1

#define MODE_RANDOM 10
#define MODE_ROTATE 11
#define MODE_TWOSIDE 12
#define MODE_RANDFLASH 13
#define MODE_ROTATE1 14
#define MODE_FLOWER 15

struct Fiber
{
   byte state;
   byte color;
   byte brightness;
};

CRGB leds[NUM_LEDS];
Fiber fibers[NUM_FIBERS];

long milli;
long refresh_time;
byte mode;

byte fade_value;
byte min_brightness;

boolean init_mode;
long mode_timer;
int mode_val;

long switch_mode_timer;
long mode_duration;


void setup() 
{
  Serial.begin(9600);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  mode = MODE_RANDOM;
  mode_duration = 500000;
  switch_mode_timer = millis() + mode_duration;

  fade_value = map(analogRead(POTI_PIN), 0, 1024, 0, 255);
  min_brightness = 0;  
    
  // delay(000);

 
}

int getLED(char ref, int index)
{
  switch(ref)
  {
    case 'l': return index%(NUM_FIBERS/2) * 4;
    case 'r': return index%(NUM_FIBERS/2) * 4 + 2;
    case 'b': return index%NUM_FIBERS * 2;
  }
}



void loop() {

  milli = millis();

  if(refresh_time < milli)
  {
   /* Serial.print("Fade val: ");
    Serial.print(fade_value);
    Serial.print("\t Poti:");
    Serial.println(analogRead(POTI_PIN));*/
    FastLED.show();
    refresh_time = milli + 35;

    fade_value = map(analogRead(POTI_PIN), 0, 1024, 0, 255);
    
    switchModes();
    
    handleFiberState();
    applyFiberState();
    
    for(int i = 0; i < NUM_FIBERS; i++)
    {
      leds[getLED('b', i)].setHSV(fibers[i].color, 255, fibers[i].brightness);
    }  
      

    //Serial.println(millis()-milli);
  } 

}

void switchModes()
{
  if(switch_mode_timer < millis())
  {
    for(int i = 0; i < NUM_FIBERS; i++)
    {
      fibers[i].state = 'x';
      fibers[i].brightness = 0;
    }
    
    min_brightness = 0;
    init_mode = false;
    
    mode += 1;
    if(mode > 14) mode = 10;
    
    switch_mode_timer = millis() + mode_duration;
    Serial.println(mode);
  }
  
}


void modeRotate1()
{    
    
   //int speed_val = map(switch_mode_timer - milli, 0,  mode_duration, 1750, 4000);
    //speed_val = 
    int speed_val = (analogRead(POTI_PIN)/100) * 300;
    Serial.println(speed_val);
      
    for(int i = 0; i < NUM_FIBERS; i++)
    {
      if(i % 2)
      { 
        int index = i + map(milli%((int)(speed_val*0.7)), 0 , speed_val * 0.7, 0, NUM_FIBERS);
        if(index >= NUM_FIBERS) index = index - NUM_FIBERS;
        
        int brightness = map(index, 0, NUM_FIBERS, 0, 255) - 127;
        brightness = sqrt(brightness*brightness) * 2 + 1;
        
        fibers[i].brightness = brightness;
        fibers[i].color = (brightness+milli/10)%255;
      }
      else
      {        
        int index = i - map((milli%speed_val), 0 , speed_val, 0, NUM_FIBERS);
        if(index < 0) index = NUM_FIBERS + index;
        
        int brightness = map(index, 0, NUM_FIBERS, 0, 255) - 127;
        brightness = sqrt(brightness*brightness) * 2 + 1;
        
        fibers[i].brightness = brightness;
        fibers[i].color = (brightness+milli/10)%255;
      }
    }       

  
  
}

void modeFlower()
{
    fade_value = map(switch_mode_timer - milli,  mode_duration, 0, 10, 40);
    Serial.println(fade_value); 
    
  if(!init_mode)
  {
    for(int i = 0; i < NUM_FIBERS; i+=4)
    {
      fibers[i].state = '+'; 
      fibers[i+1].state = '+';      
    }
    init_mode = true;
  }
}

void modeRotate()
{  
    fade_value = map(switch_mode_timer - milli,  mode_duration, 0, 32, 85);
    Serial.println(fade_value);  
    
  if(!init_mode)
  {    
    for(int i = 0; i < NUM_FIBERS; i+=3)
    {
      fibers[i].state = '+';      
    }
    init_mode = true;
  }
}

void modeRandom()
{  
  
    fade_value = map(switch_mode_timer - milli,  mode_duration, 0, 6, 28);
    Serial.println(fade_value);
    
  int active_fibers = 0;
  
  for(int i = 0; i < NUM_FIBERS; i++)
  {
    if(fibers[i].state == '+' || fibers[i].state == '-')
    {
      active_fibers++;
    }
  }
  
  if(active_fibers < NUM_FIBERS && mode_timer < millis())
  {
    byte spawn_position;
    boolean can_spawn = false;
    
    while(!can_spawn)
    {
      spawn_position = random(NUM_FIBERS);
      
      if(fibers[spawn_position].state == 0 || fibers[spawn_position].state == 'x') can_spawn = true;
    }
    
    fibers[spawn_position].state = '+';
    
    if(spawn_position % 2)
    {
      fibers[spawn_position].color = (millis()/10)%256;      
    }
    else
    {
      fibers[spawn_position].color = (millis()/10)%256;       
    }
    //fibers[spawn_position].color = random(0, 256);
    
    mode_timer = millis() + random(1);
  }
}

void modeTwoSide()
{
  
    fade_value = map(switch_mode_timer - milli,  mode_duration, 0, 10, 100);
    Serial.println(fade_value);  
    
  if(!init_mode)
  {
    min_brightness = 75;
    fibers[0].state = '+';
    init_mode = true;
  }
}


void modeRandFlash()
{
  
    fade_value = map(switch_mode_timer - milli,  mode_duration, 0, 30, 250);
    Serial.println(fade_value);  
    
    if(mode_timer <  millis())
    {
      
       for(int j = 0; j < NUM_FIBERS/2; j++)
       {
        int spawn_index = random(NUM_FIBERS);
        
        fibers[spawn_index].color = random(255);
        fibers[spawn_index].state = '+';
       }
       
      mode_timer = millis() + random(200);
    }
}

void handleFiberState()
{
  switch(mode)
  {
    case MODE_ROTATE: modeRotate();
      break;
    case MODE_RANDOM: modeRandom();
      break;
    case MODE_TWOSIDE: modeTwoSide();
      break;
    case MODE_FLOWER: modeFlower();
      break;
    case MODE_ROTATE1: modeRotate1();
      break;
    case MODE_RANDFLASH: modeRandFlash();
      break;
  }
}

void applyFiberState()
{
  
  for(int i = 0; i < NUM_FIBERS; i++)
  {
    
    /*Serial.print(fibers[i].state);
    Serial.print("|");
    Serial.print(fibers[i].brightness);
    Serial.print("\t");*/
    
    if(fibers[i].state == '+')
    {
      if(fibers[i].brightness + fade_value > 255)
      {
        fibers[i].brightness = 255;
        fibers[i].state = '-';
        onFadedIn(i);
      }
      else
      {
        fibers[i].brightness += fade_value;        
      }
    }
    else if(fibers[i].state == '-')
    {
      if(fibers[i].brightness - fade_value < min_brightness)
      {
        fibers[i].brightness = min_brightness;
        fibers[i].state = 0;
        onFadedOut(i);
      }
      else
      {
        fibers[i].brightness -= fade_value;        
      }
    }
    else if(fibers[i].state == '1')
    {
      fibers[i].brightness = 255;
    }
    else if(fibers[i].state == '0')
    {
      fibers[i].brightness = min_brightness;
    }
  }

  //Serial.println();
    
}

//Called when Fiber faded in (255)
void onFadedIn(int index)
{
  if(mode == MODE_ROTATE)
  {
    if(index%2)
    {
      if(index + 2 > NUM_FIBERS)
      {
        fibers[index + 2 - NUM_FIBERS].state = '+';
        fibers[index + 2 - NUM_FIBERS].color = ((milli/50 + map(index, 0, NUM_FIBERS, 0 , 255) ) % 255);        
      }
      else
      {        
        fibers[index + 2].state = '+'; 
        fibers[index + 2].color = ((milli/50 + map(index, 0, NUM_FIBERS, 0 , 255) )%255);  
      }
    }
    else
    {
      if(index - 2 < 0)
      {
        fibers[NUM_FIBERS + (index - 2)].state = '+';
        fibers[NUM_FIBERS + (index - 2)].color = ((milli/50 + map(index, 0, NUM_FIBERS, 0 , 255) )%255);        
      }
      else
      {        
        fibers[index - 2].state = '+'; 
        fibers[index - 2].color = ((milli/50 + map(index, 0, NUM_FIBERS, 0 , 255) )%255);
      }    
    }
  }
}

//Called when Fiber faded out (min_brightness)
void onFadedOut(int index)
{

  if(mode == MODE_FLOWER)
  {
    fibers[index].state = '+';
    fibers[index].color = (milli/50)%255;
  }
  
  if(mode == MODE_TWOSIDE)
  {
    if(index%2)
    {
       for(int i = 0; i < NUM_FIBERS - 1; i+=2)
       {
          fibers[i].state = '+';
          fibers[i].color = (milli/100)%255;
          fibers[i+1].state = 0;
          fibers[i+1].brightness = min_brightness;
       }
    }else{
       for(int i = 0; i < NUM_FIBERS - 1; i+=2)
       {
          fibers[i+1].state = '+';
          fibers[i+1].color = (milli/100 + 30)%255;
          fibers[i].state = 0;
          fibers[i].brightness = min_brightness;
       }
    }
  }
  
  if(mode == MODE_ROTATE && 0 == 1)
  {
    if(index%2)
    {
      if(index + 2 > NUM_FIBERS)
      {
        fibers[index + 2 - NUM_FIBERS].state = '+';
        fibers[index + 2 - NUM_FIBERS].color = ((milli/50)%255);        
      }
      else
      {        
        fibers[index + 2].state = '+'; 
        fibers[index + 2].color = ((milli/50)%255);  
      }
    }
    else
    {
      if(index - 2 < 0)
      {
        fibers[NUM_FIBERS + (index - 2)].state = '+';
        fibers[NUM_FIBERS + (index - 2)].color = ((milli/50)%255);        
      }
      else
      {        
        fibers[index - 2].state = '+'; 
        fibers[index - 2].color = ((milli/50)%255);
      }    
    }
  }
}
