#include "TimerDiagnostics_STM.h"

/**
 * This example shows how to set and display some timer values
 */

void setup() {
  Serial.begin(115200);
  while(!Serial)
    ;
}

void loop() {
  static bool toggle = true;

  if (toggle) {
    timer1Diagnostics.CCMR1.CC1S.set(1); // CC1 channel is configured as input, IC1 is mapped on TI1
    timer1Diagnostics.CCMR1.CC2S.set(1); // CC2 channel is configured as input, IC2 is mapped on TI2.

    timer1Diagnostics.CCER.CC1P.set(); // CC1P=1 -> noninverted channel 1
    timer1Diagnostics.CCER.CC2P.set(); // CC2P=1 -> noninverted channel 2

    timer1Diagnostics.CCMR1.IC1F.clear(); // No filter, sampling is done at fDTS
    timer1Diagnostics.CCMR1.IC2F.set(0);  // No filter, sampling is done at fDTS

    Serial.println("Timer(1) after setting 1:");
  } else {
    timer1Diagnostics.CCMR1.CC1S.set(2); // CC1 channel is configured as input, IC1 is mapped on TI2.
    timer1Diagnostics.CCMR1.CC2S.set(2); // CC2 channel is configured as input, IC2 is mapped on TI1

    timer1Diagnostics.CCER.CC1P.set(0);  // CC1P=0 -> inverted channel 1
    timer1Diagnostics.CCER.CC2P.clear(); // CC2P=0 -> inverted channel 2

    timer1Diagnostics.CCMR1.IC1F.set(); // All the filters
    timer1Diagnostics.CCMR1.IC2F.set(); // All the filters

    Serial.println("Timer(1) after setting 2:");
  }
  toggle = !toggle;
  timer1Diagnostics.println(Serial);
  Serial.println();
  delay(5000);
}
