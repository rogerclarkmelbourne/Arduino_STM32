/*
  Interactive Test Session for LeafLabs Maple

  Useful for testing Maple features and troubleshooting.
  Communicates over SerialUSB.

  This code is released into the public domain.
*/

// ASCII escape character
#define ESC       ((uint8)27)

// Default USART baud rate
#define BAUD     9600

uint8 gpio_state[BOARD_NR_GPIO_PINS];

const char* dummy_data = ("qwertyuiopasdfghjklzxcvbnmmmmmm,./1234567890-="
                          "qwertyuiopasdfghjklzxcvbnm,./1234567890");

// -- setup() and loop() ------------------------------------------------------

void setup() {
    // Set up the LED to blink
    pinMode(BOARD_LED_PIN, OUTPUT);

    // Start up the serial ports
    Serial1.begin(BAUD);
    Serial2.begin(BAUD);
    Serial3.begin(BAUD);

    // Send a message out over SerialUSB interface
    SerialUSB.println(" ");
    SerialUSB.println("    __   __             _      _");
    SerialUSB.println("   |  \\/  | __ _ _ __ | | ___| |");
    SerialUSB.println("   | |\\/| |/ _` | '_ \\| |/ _ \\ |");
    SerialUSB.println("   | |  | | (_| | |_) | |  __/_|");
    SerialUSB.println("   |_|  |_|\\__,_| .__/|_|\\___(_)");
    SerialUSB.println("                 |_|");
    SerialUSB.println("                              by leaflabs");
    SerialUSB.println("");
    SerialUSB.println("");
    SerialUSB.println("Maple interactive test program (type '?' for help)");
    SerialUSB.println("----------------------------------------------------------");
    SerialUSB.print("> ");

}

void loop () {
    toggleLED();
    delay(100);

    while (SerialUSB.available()) {
        uint8 input = SerialUSB.read();
        SerialUSB.println(input);

        switch(input) {
        case '\r':
            break;

        case ' ':
            SerialUSB.println("spacebar, nice!");
            break;

        case '?':
        case 'h':
            cmd_print_help();
            break;

        case 'u':
            SerialUSB.println("Hello World!");
            break;

        case 'w':
            Serial1.println("Hello World!");
            Serial2.println("Hello World!");
            Serial3.println("Hello World!");
            break;

        case 'm':
            cmd_serial1_serial3();
            break;

        case '.':
            while (!SerialUSB.available()) {
                Serial1.print(".");
                Serial2.print(".");
                Serial3.print(".");
                SerialUSB.print(".");
            }
            break;

        case 'n':
            cmd_adc_stats();
            break;

        case 'N':
            cmd_stressful_adc_stats();
            break;

        case 'e':
            cmd_everything();
            break;

        case 'W':
            while (!SerialUSB.available()) {
                Serial1.print(dummy_data);
                Serial2.print(dummy_data);
                Serial3.print(dummy_data);
            }
            break;

        case 'U':
            SerialUSB.println("Dumping data to USB. Press any key.");
            while (!SerialUSB.available()) {
                SerialUSB.print(dummy_data);
            }
            break;

        case 'g':
            cmd_sequential_gpio_writes();
            break;

        case 'G':
            cmd_gpio_toggling();
            break;

        case 'f':
            SerialUSB.println("Wiggling D4 as fast as possible in bursts. "
                         "Press any key.");
            pinMode(4, OUTPUT);
            while (!SerialUSB.available()) {
                fast_gpio(4);
                delay(1);
            }
            break;

        case 'p':
            cmd_sequential_pwm_test();
            break;

        case '_':
            SerialUSB.println("Delaying for 5 seconds...");
            delay(5000);
            break;

        // Be sure to update cmd_print_help() if you implement these:

        case 't':               // TODO
            SerialUSB.println("Unimplemented.");
            break;

        case 'T':               // TODO
            SerialUSB.println("Unimplemented.");
            break;

        case 's':
            cmd_servo_sweep();
            break;

        case 'd':
            SerialUSB.println("Pulling down D4, D22. Press any key.");
            pinMode(22, INPUT_PULLDOWN);
            pinMode(4, INPUT_PULLDOWN);
            while (!SerialUSB.available()) {
                continue;
            }
            SerialUSB.println("Pulling up D4, D22. Press any key.");
            pinMode(22, INPUT_PULLUP);
            pinMode(4, INPUT_PULLUP);
            while (!SerialUSB.available()) {
                continue;
            }
            SerialUSB.read();
            pinMode(4, OUTPUT);
            break;

        // Be sure to update cmd_print_help() if you implement these:

        case 'i':               // TODO
            SerialUSB.println("Unimplemented.");
            break;

        case 'I':               // TODO
            SerialUSB.println("Unimplemented.");
            break;

        case 'r':
            cmd_gpio_monitoring();
            break;

        case 'a':
            cmd_sequential_adc_reads();
            break;

        case 'b':
            cmd_board_info();
            break;

        case '+':
            cmd_gpio_qa();
            break;

        default: // -------------------------------
            SerialUSB.print("Unexpected: ");
            SerialUSB.print(input);
            SerialUSB.println(", press h for help.");
        }

        SerialUSB.print("> ");
    }
}

// -- Commands ----------------------------------------------------------------

void cmd_print_help(void) {
    SerialUSB.println("");
    SerialUSB.println("Command Listing");
    SerialUSB.println("\t?: print this menu");
    SerialUSB.println("\th: print this menu");
    SerialUSB.println("\tw: print Hello World on all 3 USARTS");
    SerialUSB.println("\tn: measure noise and do statistics");
    SerialUSB.println("\tN: measure noise and do statistics with background stuff");
    SerialUSB.println("\ta: show realtime ADC info");
    SerialUSB.println("\t.: echo '.' until new input");
    SerialUSB.println("\tu: print Hello World on USB");
    SerialUSB.println("\t_: do as little as possible for a couple seconds (delay)");
    SerialUSB.println("\tp: test all PWM channels sequentially");
    SerialUSB.println("\tW: dump data as fast as possible on all 3 USARTS");
    SerialUSB.println("\tU: dump data as fast as possible on USB");
    SerialUSB.println("\tg: toggle GPIOs sequentially");
    SerialUSB.println("\tG: toggle GPIOs at the same time");
    SerialUSB.println("\tf: toggle pin 4 as fast as possible in bursts");
    SerialUSB.println("\tr: monitor and print GPIO status changes");
    SerialUSB.println("\ts: output a sweeping servo PWM on all PWM channels");
    SerialUSB.println("\tm: output data on USART1 and USART3 with various rates");
    SerialUSB.println("\tb: print information about the board.");
    SerialUSB.println("\t+: test shield mode (for quality assurance testing)");

    SerialUSB.println("Unimplemented:");
    SerialUSB.println("\te: do everything all at once until new input");
    SerialUSB.println("\tt: output a 1khz squarewave on all GPIOs");
    SerialUSB.println("\tT: output a 1hz squarewave on all GPIOs");
    SerialUSB.println("\ti: print out a bunch of info about system state");
    SerialUSB.println("\tI: print out status of all headers");
}

void cmd_adc_stats(void) {
    SerialUSB.println("Taking ADC noise stats.");
    digitalWrite(BOARD_LED_PIN, 0);
    for (uint32 i = 0; i < BOARD_NR_ADC_PINS; i++) {
        delay(5);
        measure_adc_noise(boardADCPins[i]);
    }
}

void cmd_stressful_adc_stats(void) {
    SerialUSB.println("Taking ADC noise stats under duress.");

    for (uint32 i = 0; i < BOARD_NR_ADC_PINS; i++) {
        for (uint32 j = 0; j < BOARD_NR_PWM_PINS; j++) {
            if (boardADCPins[i] != boardPWMPins[j]) {
                pinMode(boardPWMPins[j], PWM);
                pwmWrite(boardPWMPins[j], 1000 + i);
            }
        }

        Serial1.print(dummy_data);

        measure_adc_noise(boardADCPins[i]);

        for (uint32 j = 0; j < BOARD_NR_PWM_PINS; j++) {
            if (boardADCPins[i] != boardPWMPins[j]) {
                pinMode(boardPWMPins[j], OUTPUT);
                digitalWrite(boardPWMPins[j], LOW);
            }
        }
    }
}

void cmd_everything(void) { // TODO
    // Be sure to update cmd_print_help() if you implement this.

    // print to usart
    // print to usb
    // toggle gpios
    // enable pwm
    SerialUSB.println("Unimplemented.");
}

void cmd_serial1_serial3(void) {
    HardwareSerial *serial_1_and_3[] = {&Serial1, &Serial3};

    SerialUSB.println("Testing 57600 baud on USART1 and USART3. "
                      "Press any key to stop.");
    usart_baud_test(serial_1_and_3, 2, 57600);
    SerialUSB.read();

    SerialUSB.println("Testing 115200 baud on USART1 and USART3. "
                      "Press any key to stop.");
    usart_baud_test(serial_1_and_3, 2, 115200);
    SerialUSB.read();

    SerialUSB.println("Testing 9600 baud on USART1 and USART3. "
                      "Press any key to stop.");
    usart_baud_test(serial_1_and_3, 2, 9600);
    SerialUSB.read();

    SerialUSB.println("Resetting USART1 and USART3...");
    Serial1.begin(BAUD);
    Serial3.begin(BAUD);
}

void cmd_gpio_monitoring(void) {
    SerialUSB.println("Monitoring pin state changes. Press any key to stop.");

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(i, INPUT_PULLDOWN);
        gpio_state[i] = (uint8)digitalRead(i);
    }

    while (!SerialUSB.available()) {
        for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
            if (boardUsesPin(i))
                continue;

            uint8 current_state = (uint8)digitalRead(i);
            if (current_state != gpio_state[i]) {
                SerialUSB.print("State change on pin ");
                SerialUSB.print(i, DEC);
                if (current_state) {
                    SerialUSB.println(":\tHIGH");
                } else {
                    SerialUSB.println(":\tLOW");
                }
                gpio_state[i] = current_state;
            }
        }
    }

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(i, OUTPUT);
    }
}

void cmd_sequential_adc_reads(void) {
    SerialUSB.print("Sequentially reading most ADC ports.");
    SerialUSB.println("Press any key for next port, or ESC to stop.");

    for (uint32 i = 0; i < BOARD_NR_ADC_PINS; i++) {
        if (boardUsesPin(i))
            continue;

        SerialUSB.print("Reading pin ");
        SerialUSB.print(boardADCPins[i], DEC);
        SerialUSB.println("...");
        pinMode(boardADCPins[i], INPUT_ANALOG);
        while (!SerialUSB.available()) {
            int sample = analogRead(boardADCPins[i]);
            SerialUSB.print(boardADCPins[i], DEC);
            SerialUSB.print("\t");
            SerialUSB.print(sample, DEC);
            SerialUSB.print("\t");
            SerialUSB.print("|");
            for (int j = 0; j < 4096; j += 100) {
                if (sample >= j) {
                    SerialUSB.print("#");
                } else {
                    SerialUSB.print(" ");
                }
            }
            SerialUSB.print("| ");
            for (int j = 0; j < 12; j++) {
                if (sample & (1 << (11 - j))) {
                    SerialUSB.print("1");
                } else {
                    SerialUSB.print("0");
                }
            }
            SerialUSB.println("");
        }
        pinMode(boardADCPins[i], OUTPUT);
        digitalWrite(boardADCPins[i], 0);
        if (SerialUSB.read() == ESC)
            break;
    }
}

bool test_single_pin_is_high(int high_pin, const char* err_msg) {
    bool ok = true;
    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i)) continue;

        if (digitalRead(i) == HIGH && i != high_pin) {
            SerialUSB.println();
            SerialUSB.print("\t*** FAILURE! pin ");
            SerialUSB.print(i, DEC);
            SerialUSB.print(' ');
            SerialUSB.println(err_msg);
            ok = false;
        }
    }
    return ok;
}

bool wait_for_low_transition(uint8 pin) {
    uint32 start = millis();
    while (millis() - start < 2000) {
        if (digitalRead(pin) == LOW) {
            return true;
        }
    }
    return false;
}

void cmd_gpio_qa(void) {
    bool all_pins_ok = true;
    const int not_a_pin = -1;
    SerialUSB.println("Doing QA testing for unused GPIO pins.");

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i)) continue;

        pinMode(i, INPUT);
    }

    SerialUSB.println("Waiting to start.");
    ASSERT(!boardUsesPin(0));
    while (digitalRead(0) == LOW) continue;

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i)) {
            SerialUSB.print("Skipping pin ");
            SerialUSB.println(i, DEC);
            continue;
        }
        bool pin_ok = true;
        SerialUSB.print("Checking pin ");
        SerialUSB.print(i, DEC);
        while (digitalRead(i) == LOW) continue;

        pin_ok = pin_ok && test_single_pin_is_high(i, "is also HIGH");

        if (!wait_for_low_transition(i)) {
            SerialUSB.println("Transition to low timed out; something is "
                              "very wrong.  Aborting test.");
            return;
        }

        pin_ok = pin_ok && test_single_pin_is_high(not_a_pin, "is still HIGH");

        if (pin_ok) {
            SerialUSB.println(": ok");
        }

        all_pins_ok = all_pins_ok && pin_ok;
    }

    if (all_pins_ok) {
        SerialUSB.println("Finished; test passes.");
    } else {
        SerialUSB.println("**** TEST FAILS *****");
    }

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i)) continue;

        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
        gpio_state[i] = 0;
    }
}

void cmd_sequential_gpio_writes(void) {
    SerialUSB.println("Sequentially toggling all unused pins. "
                      "Press any key for next pin, ESC to stop.");

    for (uint32 i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;

        SerialUSB.print("Toggling pin ");
        SerialUSB.print((int)i, DEC);
        SerialUSB.println("...");

        pinMode(i, OUTPUT);
        do {
            togglePin(i);
        } while (!SerialUSB.available());

        digitalWrite(i, LOW);
        if (SerialUSB.read() == ESC)
            break;
    }
}

void cmd_gpio_toggling(void) {
    SerialUSB.println("Toggling all unused pins simultaneously. "
                      "Press any key to stop.");

    for (uint32 i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(i, OUTPUT);
    }

    while (!SerialUSB.available()) {
        for (uint32 i = 0; i < BOARD_NR_GPIO_PINS; i++) {
            if (boardUsesPin(i))
                continue;
            togglePin(i);
        }
    }

    for (uint32 i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        digitalWrite(i, LOW);
    }
}

void cmd_sequential_pwm_test(void) {
    SerialUSB.println("Sequentially testing PWM on all unused pins. "
                      "Press any key for next pin, ESC to stop.");

    for (uint32 i = 0; i < BOARD_NR_PWM_PINS; i++) {
        if (boardUsesPin(i))
            continue;

        SerialUSB.print("PWM out on header D");
        SerialUSB.print(boardPWMPins[i], DEC);
        SerialUSB.println("...");
        pinMode(boardPWMPins[i], PWM);
        pwmWrite(boardPWMPins[i], 16000);

        while (!SerialUSB.available()) {
            delay(10);
        }

        pinMode(boardPWMPins[i], OUTPUT);
        digitalWrite(boardPWMPins[i], 0);
        if (SerialUSB.read() == ESC)
            break;
    }
}

void cmd_servo_sweep(void) {
    SerialUSB.println("Testing all PWM headers with a servo sweep. "
                      "Press any key to stop.");
    SerialUSB.println();

    disable_usarts();
    init_all_timers(21);

    for (uint32 i = 0; i < BOARD_NR_PWM_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(boardPWMPins[i], PWM);
        pwmWrite(boardPWMPins[i], 4000);
    }

    // 1.25ms = 4096counts = 0deg
    // 1.50ms = 4915counts = 90deg
    // 1.75ms = 5734counts = 180deg
    int rate = 4096;
    while (!SerialUSB.available()) {
        rate += 20;
        if (rate > 5734)
            rate = 4096;
        for (uint32 i = 0; i < BOARD_NR_PWM_PINS; i++) {
            if (boardUsesPin(i))
                continue;
            pwmWrite(boardPWMPins[i], rate);
        }
        delay(20);
    }

    for (uint32 i = 0; i < BOARD_NR_PWM_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(boardPWMPins[i], OUTPUT);
    }
    init_all_timers(1);
    enable_usarts();
}

void cmd_board_info(void) {     // TODO print more information
    SerialUSB.println("Board information");
    SerialUSB.println("=================");

    SerialUSB.print("* Clock speed (cycles/us): ");
    SerialUSB.println(CYCLES_PER_MICROSECOND);

    SerialUSB.print("* BOARD_LED_PIN: ");
    SerialUSB.println(BOARD_LED_PIN);

    SerialUSB.print("* BOARD_BUTTON_PIN: ");
    SerialUSB.println(BOARD_BUTTON_PIN);

    SerialUSB.print("* GPIO information (BOARD_NR_GPIO_PINS = ");
    SerialUSB.print(BOARD_NR_GPIO_PINS);
    SerialUSB.println("):");
    print_board_array("ADC pins", boardADCPins, BOARD_NR_ADC_PINS);
    print_board_array("PWM pins", boardPWMPins, BOARD_NR_PWM_PINS);
    print_board_array("Used pins", boardUsedPins, BOARD_NR_USED_PINS);
}

// -- Helper functions --------------------------------------------------------

void measure_adc_noise(uint8 pin) {
    uint16 data[100];
    float mean = 0;
    float delta = 0;
    float M2 = 0;
    pinMode(pin, INPUT_ANALOG);

    // Variance algorithm from Welford, via Knuth, by way of Wikipedia:
    // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#On-line_algorithm
    for (int i = 0; i < 100; i++) {
        data[i] = analogRead(pin);
        delta = data[i] - mean;
        mean = mean + delta / (i + 1);
        M2 = M2 + delta * (data[i] - mean);
    }

    SerialUSB.print("header: D");
    SerialUSB.print(pin, DEC);
    SerialUSB.print("\tn: ");
    SerialUSB.print(100, DEC);
    SerialUSB.print("\tmean: ");
    SerialUSB.print(mean);
    SerialUSB.print("\tvariance: ");
    SerialUSB.println(M2 / 99.0);
    pinMode(pin, OUTPUT);
}

void fast_gpio(int maple_pin) {
    gpio_dev *dev = PIN_MAP[maple_pin].gpio_device;
    uint32 bit = PIN_MAP[maple_pin].gpio_bit;

    gpio_write_bit(dev, bit, 1);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
    gpio_toggle_bit(dev, bit);
}

void usart_baud_test(HardwareSerial **serials, int n, unsigned baud) {
    for (int i = 0; i < n; i++) {
        serials[i]->begin(baud);
    }
    while (!SerialUSB.available()) {
        for (int i = 0; i < n; i++) {
            serials[i]->println(dummy_data);
            if (serials[i]->available()) {
                serials[i]->println(serials[i]->read());
                delay(1000);
            }
        }
    }
}

static uint16 init_all_timers_prescale = 0;

static void set_prescale(timer_dev *dev) {
    timer_set_prescaler(dev, init_all_timers_prescale);
}

void init_all_timers(uint16 prescale) {
    init_all_timers_prescale = prescale;
    timer_foreach(set_prescale);
}

void enable_usarts(void) {
    // FIXME generalize after USART refactor
    Serial1.begin(BAUD);
    Serial2.begin(BAUD);
    Serial3.begin(BAUD);
}

void disable_usarts(void) {
    // FIXME generalize after USART refactor
    Serial1.end();
    Serial2.end();
    Serial3.end();
}

void print_board_array(const char* msg, const uint8 arr[], int len) {
    SerialUSB.print("\t");
    SerialUSB.print(msg);
    SerialUSB.print(" (");
    SerialUSB.print(len);
    SerialUSB.print("): ");
    for (int i = 0; i < len; i++) {
        SerialUSB.print(arr[i], DEC);
        if (i < len - 1) SerialUSB.print(", ");
    }
    SerialUSB.println();
}
