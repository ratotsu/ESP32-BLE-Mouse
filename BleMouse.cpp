#include "BleMouse.h"

static const uint8_t reportMap[] = {
    0x05, 0x01,
    0x09, 0x02,
    0xA1, 0x01,

    0x85, 0x01, // REPORT ID

    0x09, 0x01,
    0xA1, 0x00,

    0x05, 0x09,
    0x19, 0x01,
    0x29, 0x03,
    0x15, 0x00,
    0x25, 0x01,
    0x95, 0x03,
    0x75, 0x01,
    0x81, 0x02,

    0x95, 0x01,
    0x75, 0x05,
    0x81, 0x03,

    0x05, 0x01,
    0x09, 0x30,
    0x09, 0x31,
    0x09, 0x38,

    0x15, 0x81,
    0x25, 0x7F,
    0x75, 0x08,
    0x95, 0x03,
    0x81, 0x06,

    0xC0,
    0xC0
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) override {
        // Evita delay - bloqueia BLE stack
    }

    void onDisconnect(NimBLEServer* pServer) override {
        NimBLEDevice::startAdvertising();
    }
};

BleMouse::BleMouse(std::string deviceName, std::string manufacturer, uint8_t batteryLevel) {
    this->deviceName = deviceName;
    this->manufacturer = manufacturer;
    this->batteryLevel = batteryLevel;
    this->pServer = nullptr;
    this->hid = nullptr;
    this->inputMouse = nullptr;
    this->initialized = false;
}

BleMouse::~BleMouse() {
    pServer = nullptr;
    hid = nullptr;
    inputMouse = nullptr;
}


void BleMouse::begin() {
    if (initialized) return;
    
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    hid = new NimBLEHIDDevice(pServer);
    inputMouse = hid->inputReport(1);

    hid->manufacturer()->setValue(manufacturer);
    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    hid->hidInfo(0x00, 0x01);
    hid->reportMap((uint8_t*)reportMap, sizeof(reportMap));
    hid->startServices();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->setAppearance(0x03C2);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMaxPreferred(0x12);
    advertising->start();
    
    initialized = true;
}

bool BleMouse::isConnected() {
    return pServer->getConnectedCount() > 0;
}

void BleMouse::move(int8_t x, int8_t y, int8_t wheel) {
    if (!isConnected()) return;

    uint8_t m[4];
    m[0] = 0x00;
    m[1] =nitialized || !isConnected() || !inputMouse) return;

    uint8_t m[4] = {0x00, x, y, wheel};
    inputMouse->setValue(m, 4);
    inputMouse->notify(
    // PRESS
    uint8_t press[4] = {b, 0, 0, 0};
    inputMouse->setValue(press, 4);
    inputMouse->notify();
    delay(10);
nitialized || !isConnected() || !inputMouse) return;

    uint8_t press[4] = {b, 0, 0, 0};
    inputMouse->setValue(press, 4);
    inputMouse->notify();
