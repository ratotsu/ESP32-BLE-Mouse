<!-- Biblioteca ESP32 BLE Mouse: permite ao ESP32 funcionar como mouse BLE HID -->

# ESP32 BLE Mouse

<!-- Descrição: Biblioteca para transformar ESP32 em mouse sem fio via BLE -->
A library that allows an ESP32 to function as a Bluetooth Low Energy (BLE) Human Interface Device (HID) mouse.

## Overview

<!-- Visão geral: Implementa mouse sem fio usando BLE, ideal para dispositivos customizados -->
This library enables the ESP32 to emulate a wireless mouse using BLE. It is suitable for building custom input devices, automation tools, and assistive hardware.

The implementation is based on BLE HID reports and handles advertising, pairing, and communication with host devices such as computers and smartphones.

## Features

<!-- Funcionalidades: Lista recursos suportados -->
* BLE HID mouse implementation
* Cursor movement (X, Y axes)
* Vertical and horizontal scrolling
* Mouse button support (left, right, middle)
* Custom device name and manufacturer
* Connection status handling

## Use Cases

<!-- Casos de uso: Aplicações possíveis -->
* Custom wireless mouse hardware
* Macro and automation devices
* Presentation controllers
* Assistive technology interfaces
* Remote control systems

## Limitations

<!-- Limitações: Restrições conhecidas -->
* BLE HID support may be unstable on some macOS and iOS devices
* Not suitable for high-frequency or low-latency input requirements
* BLE stack uses significant ESP32 memory
* Limited control over connection parameters (depends on BLE stack)

## Installation

<!-- Instalação: Como instalar no Arduino IDE e PlatformIO -->

### Arduino IDE

1. Download the repository as a ZIP file
2. Open Arduino IDE
3. Go to Sketch → Include Library → Add .ZIP Library
4. Select the downloaded file

### PlatformIO

Add the following to your `platformio.ini`:

```
lib_deps =
  T-vK/ESP32-BLE-Mouse
```

### Manual Installation

Clone the repository into your Arduino libraries folder:

```
git clone https://github.com/T-vK/ESP32-BLE-Mouse.git
```

## Quick Start

```cpp
#include <BleMouse.h>

BleMouse bleMouse("ESP32 Mouse");

void setup() {
  bleMouse.begin();
}

void loop() {
  if (bleMouse.isConnected()) {
    bleMouse.move(50, 0);
    delay(1000);
    bleMouse.move(-50, 0);
    delay(1000);
  }
}
```

## How It Works

The ESP32 operates as a BLE peripheral device implementing the HID profile.

Flow:

ESP32 → BLE Peripheral → Host Device (PC / Smartphone)

The library manages:

* BLE advertising
* Pairing and bonding
* HID report descriptors
* Input report transmission

The host interprets the ESP32 as a standard mouse device.

## Core API

### Initialization

* `begin()`
  Initializes BLE services and starts advertising

* `end()`
  Stops BLE services

### Connection

* `isConnected()`
  Returns true if a device is connected

### Mouse Control

* `move(x, y, wheel = 0, hWheel = 0)`
  Moves cursor and handles scroll

* `click(button)`
  Press and release button

* `press(button)`
  Press and hold button

* `release(button)`
  Release button

* `isPressed(button)`
  Check if button is pressed

## Internal Architecture (Based on Source Code)

The `BleMouse` class encapsulates BLE HID functionality using:

* `NimBLEDevice` for BLE stack initialization
* `NimBLEServer` for device hosting
* `NimBLEHIDDevice` for HID profile handling
* HID report descriptor defining mouse behavior

Key internal responsibilities:

* Creating HID descriptor for mouse reports
* Managing BLE server lifecycle
* Sending input reports via characteristic updates
* Tracking button states and connection status

The library abstracts BLE complexity but still depends heavily on ESP32 BLE memory configuration.

## Performance Considerations

* BLE communication introduces latency compared to wired input
* Frequent `move()` calls can increase CPU usage and BLE traffic
* Use delays or rate limiting when sending continuous input
* Ensure proper power supply for stable BLE operation

## Troubleshooting

### Device not connecting

* Ensure Bluetooth is enabled on the host
* Restart Bluetooth on the host device
* Reset the ESP32

### ESP32 resets or crashes

* Use a partition scheme with more memory (e.g., "Minimal SPIFFS")
* Avoid running other memory-heavy libraries

### Input not working correctly

* Check if device is actually connected (`isConnected()`)
* Ensure host recognizes device as HID mouse

### Issues on macOS or iOS

* BLE HID support varies by device and OS version
* Some devices may fail to pair or behave inconsistently

## Compatibility

Tested with:

* Windows
* Linux
* Android

Limited or unstable support:

* macOS
* iOS

## Contributing

Contributions are welcome. Please open issues or submit pull requests.

## License

This project is licensed under the MIT License.
