// Exemplo de uso da biblioteca BleMouse para ESP32
// Testa movimentos, cliques e scroll do mouse BLE

#include <BleMouse.h>

// Instancia objeto BleMouse com nome, fabricante e bateria
BleMouse mouse("ESP32 FULL TEST", "ESP32", 100);
// Variável para controlar timing das ações
unsigned long lastAction = 0;
// Passo atual do teste
int testStep = 0;

void setup() {
  // Inicializa comunicação serial para debug
  Serial.begin(115200);
  Serial.println("BLE Mouse FULL TEST iniciado");
  // Inicializa o mouse BLE
  mouse.begin();
}

void loop() {
  // Verifica se há conexão BLE; se não, aguarda
  if (!mouse.isConnected()) {
    Serial.println("Aguardando conexão BLE...");
    delay(1000);
    return;
  }

  // Obtém tempo atual
  unsigned long now = millis();
  
  // Controle de timing: executa ação a cada 500ms
  if (now - lastAction < 500) return; // Timing não bloqueante
  lastAction = now;

  // Switch para executar diferentes testes baseados no passo
  switch (testStep) {
    case 0: mouse.move(50, 0); Serial.println("→ Direita"); break;  // Move cursor para direita
    case 1: mouse.move(0, 50); Serial.println("↓ Baixo"); break;    // Move cursor para baixo
    case 2: mouse.move(-50, 0); Serial.println("← Esquerda"); break; // Move cursor para esquerda
    case 3: mouse.move(0, -50); Serial.println("↑ Cima"); break;     // Move cursor para cima
    case 4: mouse.move(30, 30); Serial.println("↘ Diagonal"); break; // Movimento diagonal
    case 5: mouse.move(0, 0, 3); Serial.println("⬆ Scroll up"); break;     // Scroll para cima
    case 6: mouse.move(0, 0, -3); Serial.println("⬇ Scroll down"); break;   // Scroll para baixo
    case 7: mouse.click(1); Serial.println("Click esquerdo"); break;        // Clique esquerdo
    case 8: mouse.click(2); Serial.println("Click direito"); break;         // Clique direito
    case 9: mouse.click(3); Serial.println("Click meio"); break;            // Clique do meio
    case 10:
      mouse.move(20, 20);  // Move cursor
      mouse.click(1);      // E clica
      Serial.println("Movimento + click");
      break;
    // Casos para movimento contínuo para direita
    case 11: case 12: case 13: case 14: case 15:
    case 16: case 17: case 18: case 19: case 20:
      mouse.move(10, 0);
      break;
    // Casos para movimento contínuo para baixo
    case 21: case 22: case 23: case 24: case 25:
    case 26: case 27: case 28: case 29: case 30:
      mouse.move(0, 10);
      break;
    case 31:
      Serial.println("Ciclo completo!");
      testStep = -1;  // Reinicia ciclo
      break;
  }
  
  // Incrementa passo do teste
  testStep++;
}
