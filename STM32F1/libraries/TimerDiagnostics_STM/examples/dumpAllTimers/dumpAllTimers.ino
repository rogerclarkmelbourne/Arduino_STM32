#include "TimerDiagnostics_STM.h"

/**
 * This example makes a screen dump of all the timers
 */

void setup() {
  Serial.begin(115200);
  while(!Serial)
    ;
}

void loop() {
  Serial.println("timer1:"); timer1Diagnostics.println(Serial);
  Serial.println("timer2:"); timer2Diagnostics.println(Serial);
  Serial.println("timer3:"); timer3Diagnostics.println(Serial);
  Serial.println("timer4:"); timer4Diagnostics.println(Serial);
  #ifdef STM32_HIGH_DENSITY
  Serial.println("timer5:"); timer5Diagnostics.println(Serial);
  Serial.println("timer6:"); timer6Diagnostics.println(Serial);
  Serial.println("timer7:"); timer7Diagnostics.println(Serial);
  Serial.println("timer8:"); timer8Diagnostics.println(Serial);
  #endif
  delay(5000);
}
