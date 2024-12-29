#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 5        // Pino RST do módulo RFID
#define SS_PIN 10        // Pino SDA do módulo RFID
#define LED_VERDE 8      // Porta do LED verde 
#define LED_VERMELHO 7   // Porta do LED vermelho
#define SERVO_PIN 9      // Porta do servo

MFRC522 rfid(SS_PIN, RST_PIN);
Servo meuServo;
byte uidPermitido[] = {0xE3, 0x2F, 0xAC, 0x0F}; // alterar para o seu UID

void piscarLedVermelho() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_VERMELHO, HIGH);
    delay(500);
    digitalWrite(LED_VERMELHO, LOW);
    delay(500);
  }
}

void piscarLedVerde() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_VERDE, HIGH);
    delay(500);
    digitalWrite(LED_VERDE, LOW);
    delay(500);
  }
}

bool verificarUID(byte *uidLido, byte tamanhoUID, byte *uidPermitido) {
  for (byte i = 0; i < tamanhoUID; i++) {
    if (uidLido[i] != uidPermitido[i]) {
      return false;
    }
  }
  return true;
}


void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  meuServo.attach(SERVO_PIN);
  meuServo.write(0);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_VERDE, LOW);
  Serial.println("Aproxime o cartão...");
}

void loop() {

  // Caso não tenha cartão lido, retorna
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Informa o UID do cartão
  Serial.print("UID lido: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Se o UID estiver cadastrado, abre a catraca e sinaliza a luz verde, caso não, sinaliza a luz vermelha.
  if (verificarUID(rfid.uid.uidByte, rfid.uid.size, uidPermitido)) {
    Serial.println("UID permitido! Ligando o LED verde...");
    digitalWrite(LED_VERMELHO, LOW);
    meuServo.write(90);
    piscarLedVerde();
    meuServo.write(0);
  } else {
    Serial.println("UID não permitido.");
    piscarLedVermelho();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}