#include "BleMouse.h"

// Mapa de relatório HID para mouse BLE, conforme especificação HID
static const uint8_t reportMap[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)

    0x85, 0x01,  // Report ID (1)

    0x09, 0x01,  // Usage (Pointer)
    0xA1, 0x00,  // Collection (Physical)

    0x05, 0x09,  // Usage Page (Button)
    0x19, 0x01,  // Usage Minimum (Button 1)
    0x29, 0x03,  // Usage Maximum (Button 3)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x01,  // Logical Maximum (1)
    0x95, 0x03,  // Report Count (3)
    0x75, 0x01,  // Report Size (1)
    0x81, 0x02,  // Input (Data, Variable, Absolute)

    0x95, 0x01,  // Report Count (1)
    0x75, 0x05,  // Report Size (5)
    0x81, 0x03,  // Input (Constant, Variable, Absolute)

    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x30,  // Usage (X)
    0x09, 0x31,  // Usage (Y)
    0x09, 0x38,  // Usage (Wheel)

    0x15, 0x81,  // Logical Minimum (-127)
    0x25, 0x7F,  // Logical Maximum (127)
    0x75, 0x08,  // Report Size (8)
    0x95, 0x03,  // Report Count (3)
    0x81, 0x06,  // Input (Data, Variable, Relative)

    0xC0,        // End Collection
    0xC0         // End Collection
};

// Classe de callbacks para o servidor BLE
class ServerCallbacks : public NimBLEServerCallbacks {
    // Chamado quando um dispositivo se conecta
    void onConnect(NimBLEServer* pServer) override {
        // Evita delay - bloqueia BLE stack (comentário original)
    }

    // Chamado quando um dispositivo se desconecta
    void onDisconnect(NimBLEServer* pServer) override {
        // Reinicia advertising para permitir novas conexões
        NimBLEDevice::startAdvertising();
    }
};

// Construtor da classe BleMouse
BleMouse::BleMouse(std::string deviceName, std::string manufacturer, uint8_t batteryLevel) {
    this->deviceName = deviceName;
    this->manufacturer = manufacturer;
    this->batteryLevel = batteryLevel;
    this->pServer = nullptr;
    this->hid = nullptr;
    this->inputMouse = nullptr;
    this->initialized = false;
}

// Destrutor da classe BleMouse
BleMouse::~BleMouse() {
    // Limpa ponteiros para evitar vazamentos
    pServer = nullptr;
    hid = nullptr;
    inputMouse = nullptr;
}

// Método para inicializar o mouse BLE
void BleMouse::begin() {
    if (initialized) return;  // Evita reinicialização
    
    // Inicializa dispositivo BLE com nome
    NimBLEDevice::init(deviceName);
    // Define potência de transmissão
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Cria servidor BLE
    pServer = NimBLEDevice::createServer();
    // Define callbacks para conexões
    pServer->setCallbacks(new ServerCallbacks());

    // Cria dispositivo HID
    hid = new NimBLEHIDDevice(pServer);
    // Obtém característica de entrada para relatórios do mouse
    inputMouse = hid->inputReport(1);

    // Define fabricante
    hid->manufacturer()->setValue(manufacturer);
    // Define informações PnP
    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    // Define informações HID
    hid->hidInfo(0x00, 0x01);
    // Define mapa de relatório
    hid->reportMap((uint8_t*)reportMap, sizeof(reportMap));
    // Inicia serviços HID
    hid->startServices();

    // Configura advertising BLE
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    // Adiciona UUID do serviço HID
    advertising->addServiceUUID(hid->hidService()->getUUID());
    // Define aparência como mouse
    advertising->setAppearance(0x03C2);
    // Habilita resposta de scan
    advertising->setScanResponse(true);
    // Define intervalos de advertising
    advertising->setMinPreferred(0x06);
    advertising->setMaxPreferred(0x12);
    // Inicia advertising
    advertising->start();
    
    initialized = true;
}

// Verifica se há conexão BLE ativa
bool BleMouse::isConnected() {
    return pServer->getConnectedCount() > 0;
}

// Move o cursor do mouse
void BleMouse::move(int8_t x, int8_t y, int8_t wheel) {
    if (!isConnected()) return;  // Só envia se conectado

    // Cria array de relatório: botões (0), x, y, wheel
    uint8_t m[4] = {0x00, (uint8_t)x, (uint8_t)y, (uint8_t)wheel};
    // Define valor da característica
    inputMouse->setValue(m, 4);
    // Notifica o cliente BLE
    inputMouse->notify();
}

// Simula clique de botão
void BleMouse::click(uint8_t b) {
    if (!isConnected()) return;  // Só envia se conectado

    // Relatório de pressionar botão
    uint8_t press[4] = {b, 0, 0, 0};
    inputMouse->setValue(press, 4);
    inputMouse->notify();
    // Pequeno delay para simular pressão
    delay(10);

    // Relatório de liberar botão
    uint8_t release[4] = {0, 0, 0, 0};
    inputMouse->setValue(release, 4);
    inputMouse->notify();
}
