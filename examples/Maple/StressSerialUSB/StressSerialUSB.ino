/*
 Serial Stress Test

 Connect via Serial2 (header pins D0 and D1 on the Maple) for the back channel;
 otherwise just connect with Serial and press BUT to cycle through (hold
 it for at least a second).

 The output will be pretty cryptic; see inline comments for more info.

 Note that the ONOFF test will result in a hard disconnect which crashes some
 serial port monitors and will require reopening that port after 5-10 seconds.
 
 Created 20 July 2010
 By Bryan Newbold for LeafLabs
 This code is released with no strings attached.
 
 */

#define LED_PIN 13
#define BUT_PIN 38

uint32 state = 0;
#define QUICKPRINT  0
#define BIGSTUFF    1
#define NUMBERS     2
#define SIMPLE      3
#define ONOFF       4

void setup()
{
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Set up the LED to blink 
    pinMode(LED_PIN, OUTPUT);

    // Set up the Button
    pinMode(BUT_PIN, INPUT_PULLUP);

    // Setup the back channel
    Serial2.begin(9600);
    Serial2.println("Hello world! This is the debug channel.");
}

int toggle = 0;

void loop() {
    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);
    delay(300);

    // If the button is pressed go to the next state
    if(digitalRead(BUT_PIN)) {
        while(digitalRead(BUT_PIN)) {};
        state++;
    }
    
    switch(state) {

        // sends more than a buffer worth of characters indefinately.
        // this pattern makes it easy to see dropped bytes.
        case QUICKPRINT:
            for(int i = 0; i<40; i++) {
                Serial.print('.');
                Serial.print('|');
            }
            Serial2.println(Serial.pending(),DEC);
            Serial.println();
            break;

        // Send large/log stuff
        case BIGSTUFF:
            Serial.println("01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
            Serial.println((uint32)123456789,DEC);
            Serial.println(3.1415926535);
            Serial2.println(Serial.pending(),DEC);
            break;

        // Try a bunch of different types and formats
        case NUMBERS:
            Serial.println("Numbers! -----------------------------");
            Serial2.println("Numbers! -----------------------------");
            Serial.println('1');
            Serial2.println('1');
            Serial.println(1,DEC);
            Serial2.println(1,DEC);
            Serial.println(-1,DEC);
            Serial2.println(-1,DEC);
            Serial.println(3.14159265);
            Serial2.println(3.14159265);
            Serial.println(3.14159265,9);
            Serial2.println(3.14159265,9);
            Serial.println(123456789,DEC);
            Serial2.println(123456789,DEC);
            Serial.println(-123456789,DEC);
            Serial2.println(-123456789,DEC);
            Serial.println(65535,HEX);
            Serial2.println(65535,HEX);
            Serial.println(65535,OCT);
            Serial2.println(65535,OCT);
            Serial.println(65535,BIN);
            Serial2.println(65535,BIN);
            break;

        // Very basic prints
        case SIMPLE:
            Serial2.println("Trying write('a')");
            Serial.write('a');
            Serial2.println("Trying write(\"b\")");
            Serial.write("b");
            Serial2.println("Trying print('c')");
            Serial.print('c');
            Serial2.println("Trying print(\"d\")");
            Serial.print("d");
            Serial2.println("Trying print(\"efg\")");
            Serial.print("efg");
            Serial2.println("Trying println(\"hij\\n\\r\")");
            Serial.print("hij\n\r");
            Serial.write(' ');
            Serial.println();
            Serial2.println("Trying println(123456789,DEC)");
            Serial.println(123456789,DEC);
            Serial2.println("Trying println(3.141592)");
            Serial.println(3.141592);
            Serial2.println("Trying println(\"DONE\")");
            Serial.println("DONE");
            break;

        // Disables the USB peripheral, then brings it back up a 
        // few seconds later
        case ONOFF:
            Serial2.println("Shutting down...");
            Serial.println("Shutting down...");
            Serial.end();
            Serial2.println("Waiting 4seconds...");
            delay(4000);
            Serial2.println("Starting up...");
            Serial.begin();
            Serial.println("Hello World!");
            Serial2.println("Waiting 4seconds...");
            delay(4000);
            state++;
            break;
        default:
            state = 0;
    }
} 

