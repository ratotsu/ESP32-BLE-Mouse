/**
 * @file BleMouse.h
 * @brief ESP32 BLE HID Mouse Library
 *
 * Emulates a Bluetooth Low Energy mouse on the ESP32 using the NimBLE stack.
 * Supports cursor movement, scrolling (vertical and horizontal), button
 * press/release, battery level reporting, and connection event callbacks.
 *
 * @version 2.0.0
 * @license MIT
 */

#ifndef BLE_MOUSE_H
#define BLE_MOUSE_H

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include "HIDTypes.h"
#include "sdkconfig.h"
#include <functional>

// ---------------------------------------------------------------------------
// Button bitmask constants
// ---------------------------------------------------------------------------

/** @defgroup MouseButtons Mouse button bitmask constants
 *  Pass these to press(), release(), click() and isPressed().
 *  @{
 */
constexpr uint8_t MOUSE_LEFT    = 0x01; ///< Left mouse button
constexpr uint8_t MOUSE_RIGHT   = 0x02; ///< Right mouse button
constexpr uint8_t MOUSE_MIDDLE  = 0x04; ///< Middle (scroll wheel) button
constexpr uint8_t MOUSE_BACK    = 0x08; ///< Back side button
constexpr uint8_t MOUSE_FORWARD = 0x10; ///< Forward side button
/** @} */

// ---------------------------------------------------------------------------
// BleMouse
// ---------------------------------------------------------------------------

/**
 * @brief BLE HID Mouse emulator for the ESP32.
 *
 * Instantiate once, call begin() in setup(), then use move(), press(),
 * release() and click() inside loop().
 *
 * @example
 * @code
 *   BleMouse mouse("My Mouse");
 *
 *   void setup() { mouse.begin(); }
 *
 *   void loop() {
 *     if (mouse.isConnected()) {
 *       mouse.move(10, 0);
 *       delay(500);
 *     }
 *   }
 * @endcode
 */
class BleMouse {
public:
    // -----------------------------------------------------------------------
    // Construction / destruction
    // -----------------------------------------------------------------------

    /**
     * @param deviceName   BLE device name visible to the host (max 29 chars).
     * @param manufacturer Manufacturer string reported via HID PnP descriptor.
     * @param batteryLevel Initial battery percentage (0–100).
     */
    explicit BleMouse(const std::string& deviceName  = "ESP32 Mouse",
                      const std::string& manufacturer = "ESP32",
                      uint8_t            batteryLevel = 100);

    ~BleMouse();

    // -----------------------------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------------------------

    /**
     * @brief Initialise BLE services and start advertising.
     * Safe to call multiple times — subsequent calls are no-ops.
     */
    void begin();

    /**
     * @brief Stop advertising and release BLE resources.
     * After end(), begin() may be called again to restart.
     */
    void end();

    // -----------------------------------------------------------------------
    // Connection
    // -----------------------------------------------------------------------

    /** @return true if at least one host is currently connected. */
    bool isConnected() const;

    /**
     * @brief Register a callback invoked on each new connection.
     * @param cb Callable with signature `void()`.
     */
    void setConnectCallback(std::function<void()> cb);

    /**
     * @brief Register a callback invoked each time the host disconnects.
     * The library automatically restarts advertising after disconnect.
     * @param cb Callable with signature `void()`.
     */
    void setDisconnectCallback(std::function<void()> cb);

    // -----------------------------------------------------------------------
    // Mouse control
    // -----------------------------------------------------------------------

    /**
     * @brief Send a relative mouse movement and/or scroll report.
     *
     * Values are clamped to the int8_t range (–128 … 127) by the HID layer.
     * For movements larger than 127 units, call moveLarge() instead.
     *
     * @param x      Horizontal delta (positive = right).
     * @param y      Vertical delta   (positive = down).
     * @param wheel  Vertical scroll  (positive = up on most OSes).
     * @param hWheel Horizontal scroll (positive = right).
     * @return true if the report was sent successfully.
     */
    bool move(int8_t x, int8_t y, int8_t wheel = 0, int8_t hWheel = 0);

    /**
     * @brief Send large relative movements by splitting into multiple reports.
     *
     * Useful when the required delta exceeds the –128 … 127 range.
     *
     * @param x Horizontal delta (any integer value).
     * @param y Vertical delta   (any integer value).
     * @return true if all reports were sent successfully.
     */
    bool moveLarge(int x, int y);

    /**
     * @brief Press and immediately release a button.
     * @param button Bitmask of buttons to click (e.g. MOUSE_LEFT).
     * @return true if both reports were sent successfully.
     */
    bool click(uint8_t button = MOUSE_LEFT);

    /**
     * @brief Press and hold one or more buttons.
     * Calling press() with a button already held is a no-op for that button.
     * @param button Bitmask of buttons to press.
     * @return true if the report was sent successfully.
     */
    bool press(uint8_t button = MOUSE_LEFT);

    /**
     * @brief Release one or more buttons.
     * @param button Bitmask of buttons to release.
     * @return true if the report was sent successfully.
     */
    bool release(uint8_t button = MOUSE_LEFT);

    /**
     * @brief Release all currently pressed buttons.
     * @return true if the report was sent successfully (or nothing was pressed).
     */
    bool releaseAll();

    /**
     * @brief Check whether a button is currently held.
     * @param button Single button bitmask (e.g. MOUSE_RIGHT).
     * @return true if the button is in the pressed state.
     */
    bool isPressed(uint8_t button = MOUSE_LEFT) const;

    // -----------------------------------------------------------------------
    // Battery
    // -----------------------------------------------------------------------

    /**
     * @brief Update the battery level reported to the host.
     * @param level Percentage 0–100. Values above 100 are clamped to 100.
     */
    void setBatteryLevel(uint8_t level);

    /** @return The last battery level set. */
    uint8_t getBatteryLevel() const;

private:
    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /**
     * @brief Send the current button state plus the given axis deltas.
     *
     * This is the single point where all HID input reports are actually built
     * and dispatched via NimBLE notify.
     */
    bool _sendReport(int8_t x, int8_t y, int8_t wheel, int8_t hWheel);

    // -----------------------------------------------------------------------
    // Internal server-callbacks class (defined in .cpp)
    // -----------------------------------------------------------------------
    class ServerCallbacks;

    // -----------------------------------------------------------------------
    // Members
    // -----------------------------------------------------------------------

    std::string _deviceName;
    std::string _manufacturer;
    uint8_t     _batteryLevel;

    NimBLEServer*         _pServer     = nullptr;
    NimBLEHIDDevice*      _hid         = nullptr;
    NimBLECharacteristic* _inputMouse  = nullptr;

    uint8_t  _buttons    = 0x00; ///< Bitmask of currently pressed buttons
    bool     _initialized = false;

    std::function<void()> _connectCb;
    std::function<void()> _disconnectCb;
};

#endif // BLE_MOUSE_H
