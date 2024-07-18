#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key kunci;
byte nuidPICC[4];

void setup() { 
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  for (byte i = 0; i < 6; i++) {
    kunci.keyByte[i] = 0xFF;
  }

  Serial.println(F("Kode ini memindai NUID MIFARE Classsic."));
  Serial.print(F("Menggunakan kunci berikut:"));
  cetakHex(kunci.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("Tipe PICC: "));
  MFRC522::PICC_Type tipePICC = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(tipePICC));

  if (tipePICC != MFRC522::PICC_TYPE_MIFARE_MINI &&  
      tipePICC != MFRC522::PICC_TYPE_MIFARE_1K &&
      tipePICC != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Tag Anda bukan tipe MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
      rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || 
      rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("Kartu baru terdeteksi."));

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("Tag NUID adalah:"));
    Serial.print(F("Dalam hex: "));
    cetakHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("Dalam desimal: "));
    cetakDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  } else {
    Serial.println(F("Kartu telah terbaca sebelumnya."));
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void cetakHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void cetakDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}
