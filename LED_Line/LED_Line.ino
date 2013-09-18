int d = 200; // delay in ms
int bright = 2;
boolean bFlag = 1;

int SDI = 2; //Red wire (not the red 5V wire!)
int CKI = 3; //Green wire

#define STRIP_LENGTH 64 //64 LEDs on this strip
long strip_colors[STRIP_LENGTH];

void setup() {
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  pinMode(13,OUTPUT);
  Serial.begin(57600);
  //Clear out the array
  for(int x = 0 ; x < STRIP_LENGTH ; x++)
    strip_colors[x] = 0;
}

long brightness(int b)
{
  long brightness = 0;
  for(int x = 0 ; x < 3 ; x++)
   {
    brightness <<= 8;
    brightness |= b; 
   }
    return brightness;
}

void pulseFrame(int b, int d)
{
    //Pre-fill the color array with known values
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[i]   = brightness(b);//White
  }
  post_frame(); //Push the current color frame to the strip
  delay(d);

}

void pulse(int d, int s) //d duration in ms, s number ob steps
{
     //bright=brightness(100);
        for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=s) { 
    // sets the value (range from 0 to 255):
      pulseFrame(fadeValue,0);
    // wait for 30 milliseconds to see the dimming effect    
    delay(d);                            
  } 

  // fade out from max to min in increments of 5 points:
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=s) { 
    // sets the value (range from 0 to 255):
      pulseFrame(fadeValue,0);
    // wait for 30 milliseconds to see the dimming effect    
    delay(d);                            
  } 
}

void createFrame_1()
{
    //Pre-fill the color array with known values
  for(int i=0; i<STRIP_LENGTH;i=i+3)
  {
   // strip_colors[i]   = brightness(bright);//White
    strip_colors[i] = 0xFF0000;
    strip_colors[i+1] = 0x000000; //Off
    strip_colors[i+2] = 0x000000; //Off
  }
  post_frame(); //Push the current color frame to the strip
}

void createFrame_2()
{
    //Pre-fill the color array with known values
  for(int i=0; i<STRIP_LENGTH;i=i+3)
  {
    strip_colors[i] = 0x000000;
    strip_colors[i+1] = 0xFF0000;
   // strip_colors[i+1] = brightness(bright);
    strip_colors[i+2] = 0x000000; //Off
  }
  post_frame(); //Push the current color frame to the strip
}

void createFrame_3()
{
    //Pre-fill the color array with known values
  for(int i=0; i<STRIP_LENGTH;i=i+3)
  {
    strip_colors[i] = 0x000000;
    strip_colors[i+1] = 0x000000;
    strip_colors[i+2] = 0xFF0000;
   // strip_colors[i+2] = brightness(bright);
  }
  post_frame(); //Push the current color frame to the strip
}

void coin_1()
{
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[i] = 0xFF4000;
  }
  post_frame(); //Push the current color frame to the strip
}

void damage_1()
{
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[i] = 0xFF0000;
  }
  post_frame(); //Push the current color frame to the strip
}

void star_1()
{
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[random(STRIP_LENGTH)] = 0xFFFF00;
  }
  post_frame(); //Push the current color frame to the strip
}

void star_2()
{
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[random(STRIP_LENGTH)] = 0x000000;
  }
  post_frame(); //Push the current color frame to the strip
}

void all_dark()
{
  for(int i=0; i<STRIP_LENGTH;i++)
  {
    strip_colors[i] = 0x000000;
  }
  post_frame(); //Push the current color frame to the strip
}

void moveOut()
{
    createFrame_1();
    delay(d);
    createFrame_2();
    delay(d);
    createFrame_3();
    delay(d);
}

void moveIn()
{
    createFrame_3();
    delay(d);
    createFrame_2();
    delay(d);
    createFrame_1();
    delay(d);
}
void Coin()  //one blinking
{
  for(int j=0;j<1;j++)
  {
    coin_1();
    delay(d);
    all_dark();
    delay(d);
  }
}

void Damage()
{ 
  for(int k=0;k<3;k++)
  {
    damage_1();
    delay(d/4);
    all_dark();
    delay(d/4);
  }
}

void Star()
{
    star_1();
    delay(d/4);
    star_2();
    delay(d/4);
}

void Dead()
{
  for(int l=0;l<2;l++)
  {
    moveIn();
    delay(d*4);
    all_dark();
    delay(d/4);
    damage_1();
    delay(d/4);
    all_dark();
    delay(d/4);
    damage_1();
    delay(d/4);
    all_dark();
    delay(d/4);
  }
}

void loop() {
  
  char  sw='c';

 //Xbee Debug
if(Serial.available()>0){
  
  switch(Serial.read()){
  case 'a':  //coin
  Coin();
  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(13,LOW);
  break;
  
  case 'b':  //damage
  Damage();
  break;

  case 'c':  //star
  Star();
  break;

  case 'd':  //dead
  Dead();
  break;
  
  case 'e':  //dead
  pulse(1,9);
  break;
 
  }
}

}

//Takes the current strip color array and pushes it out
void post_frame (void) {
  //Each LED requires 24 bits of data
  //MSB: R7, R6, R5..., G7, G6..., B7, B6... B0 
  //Once the 24 bits have been delivered, the IC immediately relays these bits to its neighbor
  //Pulling the clock low for 500us or more causes the IC to post the data.

  for(int LED_number = 0 ; LED_number < STRIP_LENGTH ; LED_number++) {
    long this_led_color = strip_colors[LED_number]; //24 bits of color data

    for(byte color_bit = 23 ; color_bit != 255 ; color_bit--) {
      //Feed color bit 23 first (red data MSB)
      
      digitalWrite(CKI, LOW); //Only change data when clock is low
      
      long mask = 1L << color_bit;
      //The 1'L' forces the 1 to start as a 32 bit number, otherwise it defaults to 16-bit.
      
      if(this_led_color & mask) 
        digitalWrite(SDI, HIGH);
      else
        digitalWrite(SDI, LOW);
  
      digitalWrite(CKI, HIGH); //Data is latched when clock goes high
    }
  }

  //Pull clock low to put strip into reset/post mode
  digitalWrite(CKI, LOW);
  delayMicroseconds(500); //Wait for 500us to go into reset
}

