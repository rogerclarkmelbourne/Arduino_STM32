# USB Composite library for STM32F1

## Protocols supported

- standard USB HID, with many built-in profiles, and customizable with more

- MIDI over USB

- XBox360 controller (only controller-to-host is currently supported)

- Mass storage

## Basic concepts

Start with:
```
#include <USBComposite.h>
```

The library defines several crucial objects. The central object is:

```
extern USBCompositeDevice USBComposite;
```

This controls USB device identification as well as registers the plugins that are connected to it.

Plugin objects included in the library are: 

```
extern USBHIDDevice USBHID;
extern USBMidi USBMIDI;
extern USBXBox360 XBox360;
extern USBMassStorageDevice MassStorage;
extern USBCompositeSerial CompositeSerial;
```

You can also create your own customized instances of these plugin classes or their subclasses.

If you want to make a simple (non-composite) USB device, you can just call the plugin's `begin()`
method, and it will take care of registering itself with `USBComposite` and starting up
`USBComposite`. If you want to make a composite USB device, however,
you need to control the device with `USBComposite`:

```
USBComposite.clear(); // clear any plugins previously registered
plugin1.registerComponent(); 
plugin2.registerComponent();
USBComposite.begin();
```

Of course, you may need to do some further configuring of the plugins or the `USBComposite` device
before the `USBComposite.begin()` call.

Finally, there are a number of objects that implement particular protocols for the `USBHID` plugin:
```
extern HIDMouse Mouse;
extern HIDKeyboard Keyboard;
extern HIDJoystick Joystick;
extern HIDKeyboard BootKeyboard;
```
And you can customize with more. Moreover, the `USBHID` plugin itself allows for compositing
multiple HID profiles, e.g., Mouse / Keyboard / three joysticks. 

Not all combinations will fit within the constraints of the STM32F1 USB system, and not all
combinations will be supported by all operating systems.

I recommend calling `USBComposite.setDeviceId(device)` with a device number other than the default 0x0004
to prevent collisions with Maple drivers. This call should occur before calling a `begin()` method (either
of a plugin or of `USBComposite`).

## Simple USB device configuration

A simple USB device uses a single plugin. You just need to call any setup methods for the plugin
and the `begin()` method for the plugin. For instance, to inject keyboard data, you can do:

```
USBHID.begin(HID_KEYBOARD);
```

and then call `Keyboard.print("TextToInject")` to inject keyboard data. Some plugin configurations
may require further initialization code or further code that needs to be called inside the Arduino
`loop()` function.

See the `BootKeyboard`, `midiout` and `x360` example code for this procedure.

Additionally, for backwards compatibility reasons, the `USBHID` plugin has a convenience 
`USBHID_begin_with_serial()` function which works just like `USBHID.begin()` except that it also
composites a `CompositeSerial` plugin.

However, if you want a USB device using more than one plugin, then you will NOT call the plugin's
`begin()` method.

## Memory limitations

There are 320 bytes of hardware buffer memory available after endpoint 0 is taken into account. The following 
are the default buffer memory needs of the current components:

 * USB Serial: 144 bytes
 
 * USB HID: 64 bytes
 
 * USB Mass Storage: 128 bytes
 
 * USB MIDI: 128 bytes
 
 * XBox360 Controller: 64 bytes
 
This places a limit on what combinations can be used together. For instance, HID+Mass storage+MIDI should be theoretically 
OK (320 bytes), but Serial+HID+Mass storage (336 bytes) will fail with default settings (and return false from 
USBComposite.begin()) due to lack of memory.

However, USB Serial, USB HID and USB MIDI allow you to decrease buffer sizes (and allow for more complex composite devices)
by calling:
```
CompositeSerial.setRXPacketSize(size);
CompositeSerial.setTXPacketSize(size);
USBHID.setTXPacketSize(size); // not committed yet
USBMIDI.setRXPacketSize(size); // not committed yet
USBMIDI.setTXPacketSize(size); // not committed yet
```
The maximum and default packet size is 64. Smaller packet sizes have not been thoroughly tested and may slow things down. In 
particular, for HID you should make sure your packet size is sufficient for your largest HID report. The CompositeSerial 
device also has a control channel whose 16 byte packet size is not adjustable.

Note that in the above, RX and TX are from the point of view of the MCU, not the host (i.e., RX corresponds to USB Out and TX
to USB In).

## Endpoint limitations

There is one bidirectional endpoint 0 that all endpoints share, and the hardware allows for seven more. Here are 
how many endpoints besides endpoint 0 are needed for each plugin:

* USB Serial: 3

* USB HID: 1

* USB Mass Storage: 2

* USB MIDI: 2

* XBox360 Controller: 2

When combining plugins, make sure the count of these endpoints does not exceed 7. For instance, USB Serial + USB Mass Storage + 
USB MIDI + USB HID adds up to 8, which is too much.
