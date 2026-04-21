/**
 * @file BleMouse.cpp
 * @brief ESP32 BLE HID Mouse Library — implementation
 *
 * @version 2.0.0
 * @license MIT
 */

#include "BleMouse.h"
#include <algorithm>  // std::clamp (C++17), std::min / std::max

// ---------------------------------------------------------------------------
// HID Report Descriptor
//
// Defines a relative mouse with:
//   Report ID 0x01
//   Byte 0 : Buttons  — 5 buttons (bits 0-4) + 3 padding bits
//   Byte 1 : X axis   — signed 8-bit relative
//   Byte 2 : Y axis   — signed 8-bit relative
//   Byte 3 : Wheel    — signed 8-bit vertical scroll
//   Byte 4 : HWheel   — signed 8-bit horizontal scroll
// ---------------------------------------------------------------------------

static const uint8_t kReportMap[] = {
    // Usage Page: Generic Desktop
    0x05, 0x01,
    // Usage: Mouse
    0x09, 0x02,
    // Collection: Application
    0xA1, 0x01,
        // Report ID 1
        0x85, 0x01,
        // Usage: Pointer
        0x09, 0x01,
        // Collection: Physical
        0xA1, 0x00,

            // --- Buttons (5 buttons + 3-bit padding) ---
            // Usage Page: Button
            0x05, 0x09,
            // Usage Minimum: Button 1
            0x19, 0x01,
            // Usage Maximum: Button 5
            0x29, 0x05,
            // Logical Minimum: 0
            0x15, 0x00,
            // Logical Maximum: 1
            0x25, 0x01,
            // Report Count: 5
            0x95, 0x05,
            // Report Size: 1 bit
            0x75, 0x01,
            // Input: Data, Variable, Absolute
            0x81, 0x02,
            // Padding: 3 bits
            0x95, 0x01,
            0x75, 0x03,
            // Input: Constant (padding)
            0x81, 0x03,

            // --- X / Y axes ---
            // Usage Page: Generic Desktop
            0x05, 0x01,
            // Usage: X
            0x09, 0x30,
            // Usage: Y
            0x09, 0x31,
            // Logical Minimum: -127
            0x15, 0x81,
            // Logical Maximum: 127
            0x25, 0x7F,
            // Report Size: 8 bits
            0x75, 0x08,
            // Report Count: 2
            0x95, 0x02,
            // Input: Data, Variable, Relative
            0x81, 0x06,

            // --- Vertical wheel ---
            // Usage: Wheel
            0x09, 0x38,
            // Logical Minimum: -127
            0x15, 0x81,
            // Logical Maximum: 127
            0x25, 0x7F,
            // Report Size: 8 bits
            0x75, 0x08,
            // Report Count: 1
            0x95, 0x01,
            // Input: Data, Variable, Relative
            0x81, 0x06,

            // --- Horizontal wheel (AC Pan) ---
            // Usage Page: Consumer
            0x05, 0x0C,
            // Usage: AC Pan
            0x0A, 0x38, 0x02,
            // Logical Minimum: -127
            0x15, 0x81,
            // Logical Maximum: 127
            0x25, 0x7F,
            // Report Size: 8 bits
            0x75, 0x08,
            // Report Count: 1
            0x95, 0x01,
            // Input: Data, Variable, Relative
            0x81, 0x06,

        // End Collection: Physical
        0xC0,
    // End Collection: Application
    0xC0
};

// ---------------------------------------------------------------------------
// ServerCallbacks — handles connect / disconnect events
// ---------------------------------------------------------------------------

class BleMouse::ServerCallbacks : public NimBLEServerCallbacks {
public:
    explicit ServerCallbacks(BleMouse* parent) : _parent(parent) {}

    void onConnect(NimBLEServer* /*pServer*/) override {
        if (_parent->_connectCb) {
            _parent->_connectCb();
        }
    }

    void onDisconnect(NimBLEServer* /*pServer*/) override {
        // Automatically restart advertising so the host can reconnect.
        NimBLEDevice::startAdvertising();

        if (_parent->_disconnectCb) {
            _parent->_disconnectCb();
        }
    }

private:
    BleMouse* _parent;
};

// ---------------------------------------------------------------------------
// BleMouse — public interface
// ---------------------------------------------------------------------------

BleMouse::BleMouse(const std::string& deviceName,
                   const std::string& manufacturer,
                   uint8_t            batteryLevel)
    : _deviceName(deviceName)
    , _manufacturer(manufacturer)
    , _batteryLevel(batteryLevel > 100u ? 100u : batteryLevel)
{}

BleMouse::~BleMouse() {
    end();
}

// ---------------------------------------------------------------------------

void BleMouse::begin() {
    if (_initialized) return;

    NimBLEDevice::init(_deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    _pServer = NimBLEDevice::createServer();
    _pServer->setCallbacks(new ServerCallbacks(this));

    _hid = new NimBLEHIDDevice(_pServer);
    _inputMouse = _hid->inputReport(1); // Report ID 1

    _hid->manufacturer()->setValue(_manufacturer);
    _hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    _hid->hidInfo(0x00, 0x01);
    _hid->reportMap(const_cast<uint8_t*>(kReportMap), sizeof(kReportMap));
    _hid->setBatteryLevel(_batteryLevel);
    _hid->startServices();

    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID(_hid->hidService()->getUUID());
    adv->setAppearance(0x03C2); // HID Mouse
    adv->setScanResponse(true);
    adv->setMinPreferred(0x06);
    adv->setMaxPreferred(0x12);
    adv->start();

    _initialized = true;
}

void BleMouse::end() {
    if (!_initialized) return;

    NimBLEDevice::stopAdvertising();
    NimBLEDevice::deinit(true);

    _pServer     = nullptr;
    _hid         = nullptr;
    _inputMouse  = nullptr;
    _buttons     = 0x00;
    _initialized = false;
}

// ---------------------------------------------------------------------------

bool BleMouse::isConnected() const {
    return _initialized && _pServer && (_pServer->getConnectedCount() > 0);
}

void BleMouse::setConnectCallback(std::function<void()> cb) {
    _connectCb = std::move(cb);
}

void BleMouse::setDisconnectCallback(std::function<void()> cb) {
    _disconnectCb = std::move(cb);
}

// ---------------------------------------------------------------------------

bool BleMouse::move(int8_t x, int8_t y, int8_t wheel, int8_t hWheel) {
    return _sendReport(x, y, wheel, hWheel);
}

bool BleMouse::moveLarge(int x, int y) {
    if (!isConnected()) return false;

    bool ok = true;
    while (x != 0 || y != 0) {
        // Clamp each step to the int8_t range.
        const int8_t stepX = static_cast<int8_t>(
            std::max(-127, std::min(127, x)));
        const int8_t stepY = static_cast<int8_t>(
            std::max(-127, std::min(127, y)));

        ok &= _sendReport(stepX, stepY, 0, 0);

        x -= stepX;
        y -= stepY;
    }
    return ok;
}

bool BleMouse::click(uint8_t button) {
    if (!press(button))   return false;
    delay(10);
    if (!release(button)) return false;
    return true;
}

bool BleMouse::press(uint8_t button) {
    _buttons |= button;
    return _sendReport(0, 0, 0, 0);
}

bool BleMouse::release(uint8_t button) {
    _buttons &= ~button;
    return _sendReport(0, 0, 0, 0);
}

bool BleMouse::releaseAll() {
    if (_buttons == 0x00) return true;
    _buttons = 0x00;
    return _sendReport(0, 0, 0, 0);
}

bool BleMouse::isPressed(uint8_t button) const {
    return (_buttons & button) != 0;
}

// ---------------------------------------------------------------------------

void BleMouse::setBatteryLevel(uint8_t level) {
    _batteryLevel = level > 100u ? 100u : level;
    if (_hid) {
        _hid->setBatteryLevel(_batteryLevel);
    }
}

uint8_t BleMouse::getBatteryLevel() const {
    return _batteryLevel;
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

bool BleMouse::_sendReport(int8_t x, int8_t y, int8_t wheel, int8_t hWheel) {
    if (!_initialized || !isConnected() || !_inputMouse) return false;

    // Report layout (5 bytes):
    //   [0] buttons bitmask
    //   [1] X delta
    //   [2] Y delta
    //   [3] vertical wheel
    //   [4] horizontal wheel
    const uint8_t report[5] = {
        _buttons,
        static_cast<uint8_t>(x),
        static_cast<uint8_t>(y),
        static_cast<uint8_t>(wheel),
        static_cast<uint8_t>(hWheel)
    };

    _inputMouse->setValue(report, sizeof(report));
    _inputMouse->notify();
    return true;
}
