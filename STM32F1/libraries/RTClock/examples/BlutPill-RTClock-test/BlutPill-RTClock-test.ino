/*   STM32F103C8 Blue Pill ( PC13)

  Serialport set and display RTC clock , Write by CSNOL https://github.com/csnol/STM32-Examples
  based on https://github.com/rogerclarkmelbourne/Arduino_STM32

  1. Blink on PC13 per 4s or 7s by attachAlarmInterrupt for 10 times
  2. Second counter by attachSecondsInterrpt
  3. Serial output on(41s) or off(21s) by creatAlarm
  4. change to your timezone in the sketch; .
  3. get Unix epoch time from https://www.epochconverter.com/ ;
  4. last step input the 10 bits number( example: 1503945555) to Serialport ;
  5. the clock will be reset to you wanted.

  ##  Why the 10 bits Unix epoch time be used?
****Because I wanna connect to NTP server by ESP-8266.
****in the <NTPClient.h> library. getNtpTime() will return this 10 bits Unix epoch time.
*/


#include <RTClock.h>

RTClock rtclock (RTCSEL_LSE); // initialise
int timezone = 8;      // change to your timezone
time_t tt;
time_t tt1;
tm_t mtt = { 47, 9, 13, 3, 1, 22, 30, 30 };    // init time 47+1970 = 2017  Unix epoch Time counted from 00:00:00 1 Jan 1970  
char weekday1[][7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  // 0,1,2,3,4,5,6
uint8_t dateread[11];
int globAlmCount = 0;
int lastGlobAlmCount;
int SPECAlmCount = 0;
int lastSPECAlmCount;
int i = 0;
int alarmcount = 3;
uint8_t AlarmExchange = 0;
bool dispflag = true;

#define LED_PIN PC13

// This function is called in the attachSecondsInterrpt
void SecondCount ()
{
  tt++;
}
// This function is called in the attachAlarmInterrpt
void blink ()
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  globAlmCount++;
  //tt++;
}

void OnOffSerial ()
{
  dispflag = !dispflag;
  SPECAlmCount++;
}

void setup()
{
  lastGlobAlmCount = ~globAlmCount;
  lastSPECAlmCount = ~SPECAlmCount;
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  tt = rtclock.makeTime(mtt);   
  tt1 = tt;
  rtclock.attachAlarmInterrupt(blink);// Call blink
  rtclock.attachSecondsInterrupt(SecondCount);// Call SecondCount
}

void loop()
{
  while (Serial.available())
  { dateread[i] = Serial.read();
    if (i < 11) {
      i++;
    }
    else {
      i = 0;
      tt = (dateread[0] - '0') * 1000000000 + (dateread[1] - '0') * 100000000 + (dateread[2] - '0') * 10000000 + (dateread[3] - '0') * 1000000 + (dateread[4] - '0') * 100000;
      tt += (dateread[5] - '0') * 10000 + (dateread[6] - '0') * 1000 + (dateread[7] - '0') * 100 + (dateread[8] - '0') * 10 + (dateread[9] - '0');
      tt = rtclock.TimeZone(tt, timezone);    //adjust to your local date
    }
  }
  if (lastGlobAlmCount != globAlmCount | lastSPECAlmCount != SPECAlmCount ) {
    if (globAlmCount == 10) {        // to detachAlarmInterrupt and start creatAlarm after 10 times about 110s
      rtclock.detachAlarmInterrupt();
      globAlmCount = 0;
      rtclock.createAlarm(OnOffSerial, (rtclock.getTime() + 20));   // call OnOffSerial stop output date from Serial after 2 mins
      alarmcount = 20;   //change to creatAlarm  21S close Serial output and 41s open Serial output.
    }
    else {
      lastGlobAlmCount = globAlmCount;
      lastSPECAlmCount = SPECAlmCount;
      Serial.println(" Say hello to every guys ");
      if(dispflag == false)
      Serial.println(" SPECAlarm turn Display Off ");
      switch (AlarmExchange)  {
        case 0:
          rtclock.setAlarmTime(rtclock.getTime() + alarmcount);  // reset alarm = current time + 4s for attachAlarmInterrupt,  21s for creatAlarm
          AlarmExchange = 1;
          break;
        case 1:
          rtclock.breakTime(rtclock.getTime() + alarmcount * 2, mtt); //// reset alarm = current time + 7s for attachAlarmInterrupt,  41s for creatAlarm
          rtclock.setAlarmTime(mtt);
          AlarmExchange = 0;
          break;
      }
    }
  }
  if (tt1 != tt & dispflag == true )
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
