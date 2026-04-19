#include <BleMouse.h>

BleMouse mouse("ESP32 FULL TEST", "ESP32", 100);
unsigned long lastAction = 0;
int testStep = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Mouse FULL TEST iniciado");
  mouse.begin();
}

void loop() {
  if (!mouse.isConnected()) {
    Serial.println("Aguardando conexão BLE...");
    delay(1000);
    return;
  }

  unsigned long now = millis();
  
  if (now - lastAction < 500) return; // Timing não bloqueante
  lastAction = now;

  switch (testStep) {
    case 0: mouse.move(50, 0); Serial.println("→ Direita"); break;
    case 1: mouse.move(0, 50); Serial.println("↓ Baixo"); break;
    case 2: mouse.move(-50, 0); Serial.println("← Esquerda"); break;
    case 3: mouse.move(0, -50); Serial.println("↑ Cima"); break;
    case 4: mouse.move(30, 30); Serial.println("↘ Diagonal"); break;
    case 5: mouse.move(0, 0, 3); Serial.println("⬆ Scroll up"); break;
    case 6: mouse.move(0, 0, -3); Serial.println("⬇ Scroll down"); break;
    case 7: mouse.click(1); Serial.println("Click esquerdo"); break;
    case 8: mouse.click(2); Serial.println("Click direito"); break;
    case 9: mouse.click(3); Serial.println("Click meio"); break;
    case 10:
      mouse.move(20, 20);
      mouse.click(1);
      Serial.println("Movimento + click");
      break;
    case 11: case 12: case 13: case 14: case 15:
    case 16: case 17: case 18: case 19: case 20:
      mouse.move(10, 0);
      break;
    case 21: case 22: case 23: case 24: case 25:
    case 26: case 27: case 28: case 29: case 30:
      mouse.move(0, 10);
      break;
    case 31:
      Serial.println("Ciclo completo!");
      testStep = -1;
      break;
  }
  
  testStep++;
}
