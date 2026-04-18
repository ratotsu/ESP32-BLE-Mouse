#include <BleMouse.h>

BleMouse bleMouse("ESP32 Test Mouse", "ESP32", 100);

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando BLE Mouse Test...");

  bleMouse.begin();
}

void loop() {

  // aguarda conexão
  if (!bleMouse.isConnected()) {
    Serial.println("Aguardando conexão...");
    delay(1000);
    return;
  }

  Serial.println("Conectado!");

  // =========================
  // 1. TESTE DE MOVIMENTO
  // =========================
  Serial.println("Movimento X+");
  bleMouse.move(50, 0);
  delay(1000);

  Serial.println("Movimento Y+");
  bleMouse.move(0, 50);
  delay(1000);

  Serial.println("Movimento diagonal");
  bleMouse.move(-50, -50);
  delay(1000);

  // =========================
  // 2. TESTE DE CLIQUE
  // =========================
  Serial.println("Clique esquerdo");
  bleMouse.click(MOUSE_LEFT);
  delay(1000);

  Serial.println("Clique direito");
  bleMouse.click(MOUSE_RIGHT);
  delay(1000);

  Serial.println("Clique meio");
  bleMouse.click(MOUSE_MIDDLE);
  delay(1000);

  // =========================
  // 3. TESTE DE SCROLL
  // =========================
  Serial.println("Scroll para cima");
  bleMouse.move(0, 0, 2);
  delay(1000);

  Serial.println("Scroll para baixo");
  bleMouse.move(0, 0, -2);
  delay(1000);

  // =========================
  // 4. TESTE DE COMBO
  // =========================
  Serial.println("Movimento + clique");
  bleMouse.move(30, 30);
  bleMouse.click(MOUSE_LEFT);
  delay(1000);

  Serial.println("Ciclo finalizado");
  delay(3000);
}