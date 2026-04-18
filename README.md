# ESP32 BLE Mouse

Biblioteca BLE Mouse para ESP32 (fork/fix do projeto original). Permite expor o ESP32 como um dispositivo apontador (mouse) via Bluetooth Low Energy.

## Recursos
- Emulação de movimentos X/Y
- Scroll (roda)
- Cliques (esquerdo, direito, meio)
- Exemplo de uso pronto para Arduino/ESP32

## Requisitos
- ESP32 compatível com NimBLE
- Arduino IDE ou plataforma equivalente com suporte ao ESP32
- Biblioteca NimBLE (geralmente incluída no core ESP32)

## Instalação
1. Copie a pasta da biblioteca para `Documents/Arduino/libraries/` ou instale via gerenciador de bibliotecas se publicada.
2. Abra o exemplo desejado em Arduino IDE e faça o upload para seu ESP32.

## Uso rápido
Exemplo mínimo:
- Inicialize a biblioteca com um nome e fabricante:
  - `BleMouse mouse("ESP32 FULL TEST", "ESP32", 100);`
- No `setup()` chame:
  - `mouse.begin();`
- No `loop()` verifique conexão e faça movimentos ou cliques:
  - `if (mouse.isConnected()) { mouse.move(50,0); mouse.click(1); }`

Veja exemplos completos em `exemples/` (português) incluídos no repositório.

## API principal
- [`BleMouse::BleMouse`](BleMouse.h) — construtor (nome do dispositivo, fabricante, nível da bateria).
- [`BleMouse::begin`](BleMouse.h) — inicializa o serviço HID e começa a anunciar.
- [`BleMouse::isConnected`](BleMouse.h) — retorna true se houver cliente BLE conectado.
- [`BleMouse::move`](BleMouse.h) — move o cursor: `move(int8_t x, int8_t y, int8_t wheel = 0)`.
- [`BleMouse::click`](BleMouse.h) — envia clique: `click(uint8_t b = 1)`.

(Implementação em [`BleMouse.cpp`](BleMouse.cpp))

## Exemplos incluídos
- [exemples/mouse_movimento_e_click_fix/mouse_movimento_e_click_fix.ino](exemples/mouse_movimento_e_click_fix/mouse_movimento_e_click_fix.ino) — versão alternativa/ajustada.

## Metadados da biblioteca
Configuração e versão estão em [`library.properties`](library.properties).

## Notas
- O HID report map e aparência são configurados para que o dispositivo seja reconhecido como mouse.
- Em alguns PCs pode ser necessário aguardar um pequeno delay no evento de conexão.
- Existe a posibilidade de não funcionar devido ao bitmap.

Contribuições e issues são bem-vindas. Utilize o repositório original como referência: https://github.com/ratotsu/ESP32-BLE-Mouse