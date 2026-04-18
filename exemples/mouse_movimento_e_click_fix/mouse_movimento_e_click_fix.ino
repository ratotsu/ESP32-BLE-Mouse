#include <BleMouse.h>

BleMouse mouse("ESP32 FULL TEST", "ESP32", 100);

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

  Serial.println("Conectado - iniciando testes");

  // =========================
  // 1. MOVIMENTO BÁSICO
  // =========================
  mouse.move(50, 0);      // direita
  delay(500);

  mouse.move(0, 50);      // baixo
  delay(500);

  mouse.move(-50, 0);     // esquerda
  delay(500);

  mouse.move(0, -50);     // cima
  delay(500);

  mouse.move(30, 30);     // diagonal
  delay(500);

  // =========================
  // 2. SCROLL
  // =========================
  mouse.move(0, 0, 3);    // scroll cima
  delay(500);

  mouse.move(0, 0, -3);   // scroll baixo
  delay(500);

  // =========================
  // 3. CLIQUES INDIVIDUAIS
  // =========================
  mouse.click(1);
  delay(500);

  mouse.click(2);
  delay(500);

  mouse.click(3);
  delay(500);

  // =========================
  // 4. COMBINAÇÃO DE MOVIMENTO + CLIQUE
  // =========================
  mouse.move(20, 20);
  mouse.click(1);
  delay(500);

  // =========================
  // 5. STRESS TEST (movimento contínuo)
  // =========================
  for (int i = 0; i < 10; i++) {
    mouse.move(10, 0);
    delay(100);
  }

  for (int i = 0; i < 10; i++) {
    mouse.move(0, 10);
    delay(100);
  }

  // =========================
  // 6. PAUSA ENTRE CICLOS
  // =========================
  Serial.println("Ciclo completo finalizado");
  delay(3000);
}
