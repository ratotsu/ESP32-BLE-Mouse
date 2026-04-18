#ifndef BLE_MOUSE_H
#define BLE_MOUSE_H

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#include "HIDTypes.h"
#include "sdkconfig.h"

class BleMouse {
public:
    BleMouse(std::string deviceName = "ESP32 Mouse",
             std::string manufacturer = "ESP32",
             uint8_t batteryLevel = 100);

    void begin();
    void move(int8_t x, int8_t y, int8_t wheel = 0);
    void click(uint8_t b = 1);

    bool isConnected();

private:
    NimBLEServer* pServer;
    NimBLEHIDDevice* hid;
    NimBLECharacteristic* inputMouse;

    // ✔️ ADICIONE ISSO
    std::string deviceName;
    std::string manufacturer;
    uint8_t batteryLevel;
};

#endif