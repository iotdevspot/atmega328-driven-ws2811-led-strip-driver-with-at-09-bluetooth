#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

const int PIXEL_COUNT = 75;
const int LED_STRIP_PIN = 2;
const int USER_LED_PIN = 13;
const int RELAY_PIN = 3;

char state = 'A';
char inputCommand;
byte r, g, b;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
 

void setup() {
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  pinMode(USER_LED_PIN, OUTPUT);
  
  strip.begin();
  strip.show();

  Serial.begin(115200); //modulhoz szükséges baud rate
}

void loop() {
  
  while(Serial.available())
  {
    inputCommand = Serial.read();
    //Serial1.write("Bemenet jött"+inputCommand);
    if(inputCommand <= 'F' && inputCommand >= 'A')
    {
      state = inputCommand;
      Serial.write(inputCommand);
      Serial.flush();
    }
    else
    {
        switch(inputCommand)
        {
          case 'G':
            String message;
            while(Serial.available())
            {
                message+=char(Serial.read());
            }
            int av = message.indexOf("G");
            int bv = message.lastIndexOf("G");
            r = message.substring(0,av).toInt();
            g = message.substring(av+1,bv).toInt();
            b = message.substring(bv+1).toInt();
            state = 'F';
            Serial.print("G");
            Serial.print(r);
            Serial.print(g);
            Serial.print(b);
            Serial.flush();
          break;
          case 'H':
            digitalWrite(RELAY_PIN, HIGH);
            Serial.write('H');
            Serial.flush();
          break;
          case 'I':
            digitalWrite(RELAY_PIN, LOW);
            Serial.write('I');
            Serial.flush();
          break;
          default:
            
          break;
        }
    }
  }
  startShow(state);
}


void startShow(int i) {
  //átmenet lehetne a váltásnál
  switch(i){
    case 'A': 
      colorWipe(strip.Color(255, 0, 0), 10);  // Red
      break;
    case 'B': 
      colorWipe(strip.Color(0, 255, 0), 10);  // Green
      break;
    case 'C': 
      colorWipe(strip.Color(0, 0, 255), 10);  // Blue
      break;
    case 'D': 
      rainbow(10);
      break;
    case 'E': 
      rainbowCycle(10);
      break;
    case 'F': 
      colorWipe(strip.Color(r, g, b), 10);  // Custom color
      break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
