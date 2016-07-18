#include <usb_hid_device.h>

#define lPin 33
#define bPin 32
#define BUTTON_DEBOUNCE_DELAY 1

int i=1, li=0;

void setup() {
	pinMode(lPin, OUTPUT);
	HID.begin();
}

void loop() {
	bool pressed = isButtonPressed();
    if(pressed){
		Joystick.button(i,1);
		li=i;
	}else{
		Joystick.button(i,0);
		if(li==i)i++;
		i=(i>32)?1:i;
	}
}

bool isButtonPressed() {
    if (digitalRead(bPin) == HIGH) {
        delay(BUTTON_DEBOUNCE_DELAY);
        if (digitalRead(bPin) == HIGH)
       
        return true;
    }
    return false;
}