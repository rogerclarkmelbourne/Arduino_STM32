#include "TimerDiagnostics_STM.h"


// This example shows the timer register contents before and after an analogWrite()
#define OUTPUT_PIN PA9 // this must be a pin connected to timer(1)

void setup() {
  Serial.begin(115200);
  while(!Serial)
    ;
  pinMode(PA8, OUTPUT);
}

void loop() {
  static bool digitalMode = false;

  Serial.println();
  if (digitalMode) {   
    analogWrite(OUTPUT_PIN, 128);
    Serial.println("Timer(1) after analogWrite:");
  } else {
    digitalWrite(OUTPUT_PIN, LOW);
    Serial.println("Timer(1) after digitalWrite:");
  }
  digitalMode = !digitalMode;
  timer1Diagnostics.println(Serial);

  delay(5000);
}
