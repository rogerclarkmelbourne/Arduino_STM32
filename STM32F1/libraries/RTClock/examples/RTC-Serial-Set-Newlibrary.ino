/*   STM32F103C8 Black Pill ( PB12)

  Serialport set and display RTC clock , Write by CSNOL https://github.com/csnol/STM32-Examples
  based on https://github.com/rogerclarkmelbourne/Arduino_STM32

  1. Blink on PB12 per 1s.
  2. change to your timezone in the sketch; .
  3. get Unix epoch time from https://www.epochconverter.com/ ;
  4. last step input the 10 bits number( example: 1503945555) to Serialport ;
  5. the clock will be reset to you wanted.

  ##  Why the 10 bits Unix epoch time be used?
****Because I wanna connect to NTP server by ESP-8266.
****in the <NTPClient.h> library. getNtpTime() will return this 10 bits Unix epoch time.
*/


#include <RTClock.h>

RTClock rtclock (RTCSEL_LSE); // initialise
time_t tt;
tm_t mtt = { 47, 8, 27, 0, 1, 20, 30, 30 };
char weekday1[][7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  // 0,1,2,3,4,5,6
uint8_t timeread[11];
int timezone = 8 * 3600;    // timezone -8 + DST +1 = -7  CA USA
time_t tt1;

#define LED_PIN PB12

// This function is called in the attachSecondsInterrpt
void blink ()
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  tt++;
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  tt = rtclock.makeTime(mtt);
  tt1 = tt;
  rtclock.attachSecondsInterrupt(blink);// Call blink
}


int i = 0;
void loop()
{
  while (Serial.available())
  { timeread[i] = Serial.read();
    if (i < 11) {
      i++;
    }
    else {
      i = 0;
      tt = (timeread[0] - '0') * 1000000000 + (timeread[1] - '0') * 100000000 + (timeread[2] - '0') * 10000000 + (timeread[3] - '0') * 1000000 + (timeread[4] - '0') * 100000;
      tt += (timeread[5] - '0') * 10000 + (timeread[6] - '0') * 1000 + (timeread[7] - '0') * 100 + (timeread[8] - '0') * 10 + (timeread[9] - '0') + timezone;
    }
  }
  if (tt1 != tt)
  {
    tt1 = tt;
    rtclock.breakTime(tt, mtt);
    Serial.print("Date:  ");
    Serial.print(mtt.day);
    Serial.print("- ");
    Serial.print(mtt.month);
    Serial.print("  ");
    Serial.print(mtt.year + 1970);
    Serial.print("  ");
    Serial.print(weekday1[mtt.weekday]);
    Serial.print("  Time: ");
    Serial.print(mtt.hour);
    Serial.print(" : ");
    Serial.print(mtt.minute);
    Serial.print(" : ");
    Serial.println(mtt.second);
  }
}
