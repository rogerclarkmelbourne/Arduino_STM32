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