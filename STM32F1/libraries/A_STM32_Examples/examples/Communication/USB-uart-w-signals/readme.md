
# USB-serial (uart), esp8266 example 

The sketch stm32usb-serial-esp8266.ino is an example of
bridging USB-Serial to uart(1) 

## usb-serial-uart

In setup() first setup the uart port with baud rate and line discipline 
Serial1.begin(baud, disclipine) e.g.

``` Serial1.begin(115200, SERIAL_8N1);  ```

The sketch implements the handling of DTR, RTS signals from the host. 
But do note that this disables the DTR 'LEAF' magic sequence that resets the mcu from the host. 
You can define the DTR, RTS pins with the global variables dtrPIN, rtsPIN. This would allow controlling 
of these pins from the host.

In addition, it replaces the usb setup hook that process usb-cdc-acm commands from the host. This is to flag 
the event that SetLineCoding command has been received.

The bulk of the bridging is done in loop(). it reads from usb-serial writes to Serial1 an vice versa, 
toggling the dtr, rts pins between the transfers and in addition, configures the uart baud rate if the host 
sends a SetLineCoding command.

## interfacing with ESP-01 - ESP8266

I've actually used this to interface an ESP-01 (esp8266) with an AT rom.
That makes it rather convenient to test esp8266 functionality using this as a usb-serial dongle.

Interfacing with ESP8266 is beyond the scope of this example and document, but just that I've actually used 
it in that context.

There are many examples on the web just needs one to search for the keywords like ESP-01, ESP8266. one of 
the examples found at the time of writing is:

[Getting Started With the ESP8266 ESP-01](https://www.instructables.com/id/Getting-Started-With-the-ESP8266-ESP-01/) 

[ESP8266 Non-OS AT Instruction Set](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf)

[https://www.espressif.com/en/support/download/documents](https://www.espressif.com/en/support/download/documents)

It is also possible to program an ESP-01 (e.g. flash an AT ROM or some other rom). But it is beyond the 
scope of this document. A possible utility for that purpose is esptool. There are some connection specifics 
but one should refer to expressif documents or do a web search for examples. In addition, refer to 
programming/flashing another stm32 over uart section below about disabling the command processor.

[https://github.com/espressif/esptool](https://github.com/espressif/esptool)  
[https://medium.com/@aallan/getting-started-with-the-esp8266-270e30feb4d1](https://medium.com/@aallan/getting-started-with-the-esp8266-270e30feb4d1)  
(note that links may change over time and become invalid)

### esp8266 AT command processor

You can use the usb-serial-uart bridge to connect to an ESP-01 - ESP8266 and simply type, send the relevant 
AT commands. the commands would be funnelled back and fro between the host - stm32f103 (e.g. blue pill) - 
esp8266 (esp-01)

As i found it getting a little tedious doing so, i made a little 'command processor' that hooks into the 
usb-serial bridge.

To activate the command processor:  
enter +++  
and pause for 1/2 second

you would see the response '+++' after the pause and you are in 'command' mode
take a look at code of chkcmd() function. i'd recommend editing the codes changing that '+' to any other 
characters of your preference. this is because the esp8266 at rom also use +++ as an escape sequence and 
this may disrupt certain operations such as when you are connected say on a wifi-link and you want to drop 
into the esp8266 command shell. redefine that by changing the #define ESCSEQCHAR 

The commands are single character (case sensitive) commands optionally followed by arguments  
and ``<enter>`` key (i.e. either carriage return \r or line feed \n )  
an unrecognised command exits 'command mode' (response <+++)

* R reset (esp8266)  
this toggle rtsPIN low for 500ms and bring that back to high

* P program mode  
this toggles dtrPIN low

* p run mode  
this toggles dtrPIN high

* D disconnect from AP

* i print ip address

* M configure MDNS

* L"ssid","password" logon to AP  
logon to AP with ssid and password. the double quotes are needed

* W start server mode

* s{channel}{message} send some characters (max 18 chars)  
e.g. s0hello   -   sends hello on channel 0

* S{channel},{length}  
e.g. S0,100 sends the next 100 characters on channel 0

* T{channel} send some characters, timeout 5s (see function bufsend())  
e.g. enter T0 ``<enter>``, you have 5s to type your message, transmit on timeout, channel 0

* c{channel} close channel e.g. c0

* k sendlf  
esp8266 AT commands specifially requires ``<cr><lf>`` as end of line markers. 
Some terminals programs cannot make that translation on the host, most send just ``<cr>``. toggling this 
makes it send ``<cr><lf>`` when only ``<cr>`` is received. this is a toggle (i.e. flip/flop), reverses with 
each toggle

* b{baud} set the baud rate e.g. b115200  
this is intended to change the baud rate after starting up. the initial baud rade is done in setup() hardcoded. 

this may be useful for instance esp8266 boots in 'program' mode with a non-standard baud of 74880, while in 'run' 
mode it is 115200 8N1. while meddling around with a esp-01, i changed the baud to 74880 using 'b74880' command, 
used the 'P' command to set dtrPIN low, then 'R' to toggle a reset on esp8266. esp8266 then boots into the 
'program' mode, the prompts are legible instead of garbled due to a baud mismatch. i then switch out of 
serial monitor on the host and run esptool to flash a rom on the esp8266. Again, this is literally beyond 
the scope of this document, but this note here is just a tip on how i did it successfully in one of the attempts.

The AT commands are based on a version of AT rom from expressif, they may be different between versions or have 
changed. Refer to expressif documents for the AT commands reference
[ESP8266 Non-OS AT Instruction Set](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf)

[https://www.espressif.com/en/support/download/documents](https://www.espressif.com/en/support/download/documents)

(note that links may change over time and become invalid)
and the codes - function docmd() for more details. 

## Programming/flashing another stm32 over uart

This sketch can be used to program/flash another stm32(f103) over uart1. 
But programming stm32 requires an 8E1 line discipline. This should be defined in setup. e.g.

```
void setup() {
//AN3155: usart boot loader requires even parity  
Serial1.begin(115200,SERIAL_8E1);
```

In addition disable the command processor in the codes in loop() to prevent accidental activation. Though it 
may probably work even if the command processor is left there.

```
void loop() {
...
	if(Serial.available()) {
		digitalWrite(ledPin, HIGH); // blink the led for traffic
		// command processor
		// chkcmd();  //<--- comment this deactivate the command processor
		while(Serial.available()) {
			char c = Serial.read();
			Serial1.write(c);
			if(c == 13 && fsendlf) Serial1.write(10);
		}
	}
```
Set BOOT0 on target device and swap the pins PA9 (TxD), PA10 (RxD) connecting between the bridge device - 
target device. power up the target device, normally it is simply connect 3v3 and gnd

relevant docs and utilities  
- [USART protocol used in the STM32 bootloader](https://www.st.com/resource/en/application_note/cd00264342.pdf)  
- [STM32 microcontroller system memory boot mode](https://www.st.com/resource/en/application_note/cd00167594.pdf)  
- [https://github.com/jsnyder/stm32loader](https://github.com/jsnyder/stm32loader)  
- [https://medium.com/@paramaggarwal/programming-an-stm32f103-board-using-usb-port-blue-pill-953cec0dbc86](https://medium.com/@paramaggarwal/programming-an-stm32f103-board-using-usb-port-blue-pill-953cec0dbc86)  
(note that links may change over time and become invalid)
other than stm32loader there are other utilities including from ST that'd probably work. One'd just need to 
do a web search for them or browse ST's webs.

example sketch and this readme by ag123 - mit license - 2019








