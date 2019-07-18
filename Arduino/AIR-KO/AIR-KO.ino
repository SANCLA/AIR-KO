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
#define ALARMM 00

#define CLK 2                         // TM1637 pin CLK (CLOCK DISPLAY)
#define DIO 3                         // TM1637 pin DIO (CLOCK DISPLAY)
#define buzzer 10                      // Buzzer / continuous beep
#define LedAlarm 5                    // Alarm LED (integrated in button)
#define SW_HOUR A1                     // Button to adjust time, hour++
#define SW_MIN A2                     // Button to adjust time, minute++
#define LedAlarmOK 4                   // Alarm LED (integrated in button)
int BtnAlarmOff = 6; 
int BtnAlarmOffState = 0;
int AlarmAcknowledged;

//Stroom: lang = neg, kort = posm    0

TM1637 tm1637(CLK, DIO);
RTC_DS1307 RTC;                                  // RTC DS1307 use SDA(A4) and SCL(A5) pins

bool dp = true;                                  // digital points
bool rep = false;
uint8_t m;                                       // minutes
uint8_t h;                                       // hours
uint8_t tmp;
uint32_t vr = 0;                                 // time variable
//---------------------------------------------------------------------------------------------------------
void setup()
{
   
  Serial.begin(115200);
  Serial.println("Serial I AM OK!!!");
  pinMode(buzzer, OUTPUT);
  pinMode(LedAlarm, OUTPUT);
  pinMode(BtnAlarmOff, INPUT);
  pinMode(LedAlarmOK, OUTPUT);
  pinMode(SW_HOUR, INPUT_PULLUP);
  pinMode(SW_MIN, INPUT_PULLUP);  
  digitalWrite(BtnAlarmOff, HIGH);

  Wire.begin();                                  // Initiate the Wire library and join the I2C bus

  RTC.begin();                        // Initiate the RTC library
//  RTC.adjust(DateTime(__DATE__, __TIME__));
 if(!RTC.isrunning())
 {
 RTC.adjust(DateTime(__DATE__,__TIME__));
 }

  tm1637.init();                                 // Initiate the TM1637 library
  tm1637.set(BRIGHTEST);                         // BRIGHT_TYPICAL = 2, BRIGHT_DARKEST = 0, BRIGHTEST = 7
}
//---------------------------------------------------------------------------------------------------------
void loop()
{
    show_time();
    tm1637.point(dp);  

  if (read_button(SW_HOUR) > 0) {
    DateTime now = RTC.now();
    tmp = now.hour();
    if (tmp == 23)
      tmp = 0;
    else
      tmp++;
    RTC.adjust(DateTime(now.year(), now.year(), now.day(), tmp, now.minute(), now.second()));
    show_time();
    delay(10);
  }

  if (read_button(SW_MIN) > 0) {
    DateTime now = RTC.now();
    tmp = now.minute();
    if (tmp == 59)
      tmp = 0;
    else
      tmp++;
    RTC.adjust(DateTime(now.year(), now.year(), now.day(), now.hour(), tmp, 0));
    show_time();
    delay(10);
  }

  if (rep) {
    if (digitalRead(SW_HOUR) && digitalRead(SW_MIN))
      rep = false;
  }

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
      }
     else if (BtnAlarmOffState == LOW)
      {
        AlarmAcknowledged = 1;
        digitalWrite(buzzer,LOW);
        digitalWrite(LedAlarmOK, HIGH);
        delay(125);   
        digitalWrite(LedAlarmOK, LOW);
        delay(125); 
      }

  }else{
    digitalWrite(buzzer,LOW);
    AlarmAcknowledged = 0;
    delay(250);
  }
digitalWrite(LedAlarmOK, HIGH);  
}
//---------------------------------------------------------------------------------------------------------
void show_time()                                 // Time Format -> HH : MM
{
  DateTime now = RTC.now();

  h = now.hour();
  m = now.minute();

  if (h < 10)                                    // leading zero
    tm1637.display(0x00, 0x7f);
  else
    tm1637.display(0, h / 10);
  tm1637.display(1, h % 10);
  tm1637.display(2, m / 10);
  tm1637.display(3, m % 10);
}
//---------------------------------------------------------------------------------------------------------
int read_button (uint8_t pin_wbutton) {     // push button state, return: 0(very fast), 1(fast), 2(slow)
  uint32_t vr_start = 0;
  uint32_t vr_button = 0;
  uint8_t  wposition = 0;
  uint16_t wzab;

  if (rep)
    wzab = 200;
  else
    wzab = 1000;

  vr_start = millis();
  while (digitalRead(pin_wbutton) == LOW) {
    vr_button = millis() - vr_start;
    if (vr_button > wzab) {
      rep = true;
      break;
    }
  }
  if (vr_button < 30)                       // 'nothing' for debounce
    wposition = 0;
  else if (vr_button > 100)                 // slow push button
    wposition = 2;
  else wposition = 1;                       // fast push button

  return wposition;
}
