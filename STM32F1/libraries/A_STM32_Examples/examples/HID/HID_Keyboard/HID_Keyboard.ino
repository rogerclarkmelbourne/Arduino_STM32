#define bPin 32
#define lPin 33
#define BUTTON_DEBOUNCE_DELAY 1

bool isButtonPressed();

void setup() {

  pinMode(lPin, OUTPUT);
  pinMode(bPin, INPUT);
}

void loop() {
  //delay(1000);
  if(isButtonPressed()){
    Keyboard.write(KEY_LEFT_GUI);
    //Keyboard.print("Hello World !!!\n");
  }
}

bool isButtonPressed() {
    if (digitalRead(bPin) == HIGH) {
        delay(BUTTON_DEBOUNCE_DELAY);
        while (digitalRead(bPin) == HIGH)
            ;
        return true;
    }
    return false;
}


