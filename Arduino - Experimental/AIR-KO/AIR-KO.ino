/*
  Project   :   AIR-KO
  Author    :   SANCLA Sander Claassen
  Site      :   https://github.com/SANCLA/AIR-KO
  ---------------------------------------------------------------------------------------------------------*/
// meh

#include <Wire.h>
#include "RTClib.h"                           // https://github.com/adafruit/RTClib
#include "TM1637.h"                           // https://github.com/avishorp/TM1637
//#include <softwareserial.h>
//fdff#include <dfplayer_mini_mp3.h>


#define ALARMH 17
#define ALARMM 00

#define CLK 2                                   // TM1637 pin CLK (CLOCK DISPLAY)
#define DIO 3                                   // TM1637 pin DIO (CLOCK DISPLAY)
#define buzzer 10                               // Buzzer / continuous beep
#define LED_RedLedInButton 5                    // Alarm LED (integrated in button)
#define BTN_AdjustHours A1                      // Button to adjust time, hour++
#define BTN_AdjustMinutes A2                    // Button to adjust time, minute++
#define LED_GreenLedOnTop 4                     // Alarm LED (integrated in button)
#define DS18B20 8                               // Feature DS18B20 temperature sensor

int BTN_AlarmOff = 6;                           // Alarm button define (but then as int instead of define)
//int VAR_AlarmState = 0;
int AlarmAcknowledged; // Variable to store if one has pushed the alarm acknowledge button (BTN_AlarmOff)
int VAR_ShowTemp = 0; // Show temp every 5 seconds
//Stroom: lang = neg, kort = posm    0

TM1637 tm1637(CLK, DIO);
RTC_DS1307 RTC;                                  // RTC DS1307 use SDA(A4) and SCL(A5) pins

bool dp = true;                                  // digital points
bool rep = false;
uint8_t m;                                       // minutes
uint8_t h;                                       // hours
uint8_t tmp;                                     // Temporary variable to adjust the time
uint32_t vr = 0;                                 // time variable

//---------------------------------------------------------------------------------------------------------
void setup()
{

  Serial.begin(115200);
  Serial.println("Serial I AM OK!!!");
  pinMode(buzzer, OUTPUT);
  pinMode(LED_RedLedInButton, OUTPUT);
  pinMode(BTN_AlarmOff, INPUT);
  pinMode(LED_GreenLedOnTop, OUTPUT);
  pinMode(BTN_AdjustHours, INPUT_PULLUP);
  pinMode(BTN_AdjustMinutes, INPUT_PULLUP);
  digitalWrite(BTN_AlarmOff, HIGH);

  Wire.begin();                                   // Initiate the Wire library and join the I2C bus

  RTC.begin();                        // Initiate the RTC library
  //  RTC.adjust(DateTime(__DATE__, __TIME__));
  if (!RTC.isrunning())
  {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  tm1637.init();                                  // Initiate the TM1637 library
  tm1637.set(BRIGHT_TYPICAL);                     // BRIGHT_TYPICAL = 2, BRIGHT_DARKEST = 0, BRIGHTEST = 7
}
//---------------------------------------------------------------------------------------------------------
void loop() {
  if (millis() - vr > 1000) {                      // time refresh (and digital points blink): every 0,5 sec
  show_time();
    tm1637.point(dp);
    dp = !dp;
    vr = millis();
  }

  if (read_button(BTN_AdjustHours) > 0) {
  DateTime now = RTC.now();
    tmp = now.hour();
    if (tmp == 23)
      tmp = 0;
    else
      tmp++;
    RTC.adjust(DateTime(now.year(), now.year(), now.day(), tmp, now.minute(), now.second()));
    show_time();
    delay(10);
  } else if (read_button(BTN_AdjustMinutes) > 0) {
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
  if (digitalRead(BTN_AdjustHours) && digitalRead(BTN_AdjustMinutes))
      rep = false;
  }

  if (h == ALARMH && m == ALARMM) // ALARM tijd bereikt, alarm gaat af
{
  if ((digitalRead(BTN_AlarmOff) == HIGH) && (AlarmAcknowledged == 0)) // Alarm tijd en uitknop is niet ingedrukt
    {
      show_time();
      digitalWrite(buzzer, HIGH);
      digitalWrite(LED_GreenLedOnTop, HIGH);
      digitalWrite(LED_RedLedInButton, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      digitalWrite(LED_RedLedInButton, LOW);
      delay(125);
      tm1637.clearDisplay();
//      display.setBrightness(7, false);  // Turn off
      digitalWrite(LED_GreenLedOnTop, HIGH);
      digitalWrite(LED_RedLedInButton, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      digitalWrite(LED_RedLedInButton, LOW);
      delay(125);      
    }
    else if ((digitalRead(BTN_AlarmOff) == LOW) && (AlarmAcknowledged == 0)) // Alarm tijd en uitknop wordt ingedrukt
    {
      AlarmAcknowledged = 1;
      digitalWrite(buzzer, LOW);

      tm1637.display(0, h / 10);
      tm1637.display(1, h % 10);
      tm1637.display(2, m / 10);
      tm1637.display(3, m % 10);
      
      digitalWrite(LED_GreenLedOnTop, HIGH);
      digitalWrite(LED_RedLedInButton, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      digitalWrite(LED_RedLedInButton, LOW);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, HIGH);
      digitalWrite(LED_RedLedInButton, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      digitalWrite(LED_RedLedInButton, LOW);
      delay(125);      
    }
    else // Alarm is uitschakeld maar nog wel alarm tijd
    {
      digitalWrite(buzzer, LOW);
      digitalWrite(LED_RedLedInButton, LOW);
      digitalWrite(LED_GreenLedOnTop, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, HIGH);
      delay(125);
      digitalWrite(LED_GreenLedOnTop, LOW);
      delay(125);      
    }
  } else { // Geen alarmtijd meer, alles netjes achterlaten
    digitalWrite(buzzer, LOW);
    digitalWrite(LED_GreenLedOnTop, HIGH);
    digitalWrite(LED_RedLedInButton, LOW);
    AlarmAcknowledged = 0;
  }

}
//---------------------------------------------------------------------------------------------------------
void sound_alarm()                                 // Sound the alarm boys!
{
  digitalWrite(buzzer, HIGH);
  digitalWrite(LED_GreenLedOnTop, LOW);
  digitalWrite(LED_RedLedInButton, HIGH);
  delay(125);
  digitalWrite(LED_RedLedInButton, LOW);
  delay(125);
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

//void showTemp()
//{
//  sensors.requestTemperatures();  //Request temperature from the sensor.
//  i = float(sensors.getTempCByIndex(0));
//  tm1637.showNumberDec(i);
//
//}
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
