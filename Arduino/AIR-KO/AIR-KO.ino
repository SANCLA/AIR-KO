/* name      : clock_4d_led_01_1.ino
   purpose   : Clock with 4 digit LED display (TM1637), using Arduino and RTC DS1307
   author    : LZ2WSG Veselin Georgiev
   site      : http://kn34pc.com/construct/lz2wsg_arduino_clock_4d_led.html
   date      : 2-12-2018
   revision  : v1.01.1
   used info : http://www.manelsoft.com/projects/arduino_ds1307_clock.aspx
             : http://playground.arduino.cc/Main/TM1637
             : http://www.ad7c.com/projects/ad9850-dds-vfo/
  ---------------------------------------------------------------------------------------------------------*/
// meh

#include <Wire.h>
#include "RTClib.h"                   // https://github.com/adafruit/RTClib
#include "TM1637.h"                   // https://github.com/avishorp/TM1637

#define CLK 2                         // TM1637 pin CLK
#define DIO 3                         // TM1637 pin DIO
#define buzzer 8                      // Buzzer / continuous beep

TM1637 tm1637(CLK, DIO);
RTC_DS1307 RTC;                       // RTC DS1307 use SDA(A4) and SCL(A5) pins

uint8_t m;                            // minutes
uint8_t h;                            // hours
bool dp = true;                       // digital points

void setup()
{
  Wire.begin();                       // Initiate the Wire library and join the I2C bus

  pinMode(13, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
     Serial.println("Serial I AM OK!!!"); // so I can keep track of what is loaded

  RTC.begin();                        // Initiate the RTC library
//  RTC.adjust(DateTime(__DATE__, __TIME__));
 if(!RTC.isrunning())
 {
 RTC.adjust(DateTime(__DATE__,__TIME__));
 }

  tm1637.init();
  tm1637.set(BRIGHTEST);              // BRIGHT_TYPICAL = 2, BRIGHT_DARKEST = 0, BRIGHTEST = 7
}


void loop()
{

  DateTime now = RTC.now();
  Serial.begin(115200);
            Serial.print(now.hour(), DEC);
            Serial.print(':');
            Serial.print(now.minute(), DEC);
            Serial.print(':');
            Serial.print(now.second(), DEC);
            Serial.println();

  h = now.hour();
  m = now.minute();

  tm1637.display(0, h / 10);          // Time Format -> HH : MM
  tm1637.display(1, h % 10);
  tm1637.display(2, m / 10);
  tm1637.display(3, m % 10);

  tm1637.point(dp);
  dp = !dp;                           // time refresh (and digital points blink) 0,5 every sec
  
  delay(500);
  
if(h == 15 && m == 27) 
  {
   digitalWrite(buzzer,HIGH);
  }else
  {
   digitalWrite(buzzer,LOW);
  }
  
}
