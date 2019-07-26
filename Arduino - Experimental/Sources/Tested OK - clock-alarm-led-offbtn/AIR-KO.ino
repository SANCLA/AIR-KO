/* 
Project   :   AIR-KO
Author    :   SANCLA Sander Claassen
Site      :   https://github.com/SANCLA/AIR-KO
  ---------------------------------------------------------------------------------------------------------*/
// meh

#include <Wire.h>
#include "RTClib.h"                   // https://github.com/adafruit/RTClib
#include "TM1637.h"                   // https://github.com/avishorp/TM1637

#define ALARMH 17
#define ALARMM 19

#define CLK 2                         // TM1637 pin CLK
#define DIO 3                         // TM1637 pin DIO
#define buzzer 4                      // Buzzer / continuous beep
#define LedAlarm 5                    // Alarm LED (integrated in button)
//#define BtnAlarmOff 6            // Button to switch alarm off
#define BtnHr A0                     // Button to adjust time, hour++
#define BtnMn A1                     // Button to adjust time, minute++
#define LedAlarmOK 10                   // Alarm LED (integrated in button)
int BtnAlarmOff = 6; 
int BtnAlarmOffState = 0;
int AlarmAcknowledged;

TM1637 tm1637(CLK, DIO);
RTC_DS1307 RTC;                       // RTC DS1307 use SDA(A4) and SCL(A5) pins

bool dp = true;                                  // digital points
bool rep = false;
uint8_t m;                                       // minutes
uint8_t h;                                       // hours
uint8_t tmp;
uint32_t vr = 0;                                 // time variable

void setup()
{
  Wire.begin();                       // Initiate the Wire library and join the I2C bus

  pinMode(buzzer, OUTPUT);
  pinMode(LedAlarm, OUTPUT);
  pinMode(BtnAlarmOff, INPUT);
  pinMode(BtnHr, INPUT_PULLUP);
  pinMode(BtnMn, INPUT_PULLUP); 
  pinMode(LedAlarmOK, OUTPUT);
  digitalWrite(BtnAlarmOff, HIGH);
   
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


 if(h == ALARMH && m == ALARMM)
  {
    BtnAlarmOffState = digitalRead(BtnAlarmOff);
    if ((BtnAlarmOffState == HIGH) && (AlarmAcknowledged == 0)) //ALARM
      {
        digitalWrite(buzzer,HIGH);
        digitalWrite(LedAlarmOK, LOW);
        digitalWrite(LedAlarm, HIGH);
        delay(125);
        digitalWrite(LedAlarm, LOW);
        delay(125);
        digitalWrite(LedAlarm, HIGH);
        delay(125);
        digitalWrite(LedAlarm, LOW);
        delay(125);       
      }
     else if (BtnAlarmOffState == LOW)
      {
        AlarmAcknowledged = 1;
        digitalWrite(buzzer,LOW);
        digitalWrite(LedAlarmOK, HIGH);
        delay(500);
      }
     else 
      {
        digitalWrite(LedAlarmOK, HIGH);
        delay(250);   
        digitalWrite(LedAlarmOK, LOW);
        delay(250);    
     }
  }else{
    AlarmAcknowledged = 0;
    delay(500);
  }
  
}
