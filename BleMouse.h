#ifndef BLE_MOUSE_H
#define BLE_MOUSE_H

// Inclusões necessárias para BLE e HID no ESP32 usando NimBLE
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#include "HIDTypes.h"
#include "sdkconfig.h"

// Classe principal para implementar um mouse BLE no ESP32
class BleMouse {
public:
    // Construtor: inicializa o mouse BLE com nome do dispositivo, fabricante e nível de bateria
    BleMouse(std::string deviceName = "ESP32 Mouse",
             std::string manufacturer = "ESP32",
             uint8_t batteryLevel = 100);
    // Destrutor: limpa ponteiros
    ~BleMouse();

    // Inicializa o servidor BLE e serviços HID
    void begin();
    // Move o cursor do mouse (x, y) e roda (wheel)
    void move(int8_t x, int8_t y, int8_t wheel = 0);
    // Simula clique de botão (1=esquerdo, 2=direito, 3=meio)
    void click(uint8_t b = 1);
    // Verifica se há conexão BLE ativa
    bool isConnected();

private:
    // Ponteiro para o servidor BLE
    NimBLEServer* pServer;
    // Ponteiro para o dispositivo HID
    NimBLEHIDDevice* hid;
    // Característica de entrada para relatórios do mouse
    NimBLECharacteristic* inputMouse;
    // Flag para indicar se foi inicializado
    bool initialized;

    // Atributos do dispositivo
    std::string deviceName;
    std::string manufacturer;
    uint8_t batteryLevel;
};

#endif

#endif