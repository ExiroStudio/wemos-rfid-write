#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D2  // Pin untuk Reset
#define SS_PIN D1   // Pin untuk Slave Select (SDA)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Buat instance MFRC522

// Default key untuk otentikasi
MFRC522::MIFARE_Key key;

const char* data = "TKJ-C";

void setup() {
  Serial.begin(115200);  // Inisialisasi komunikasi serial dengan PC
  while (!Serial);       // Tunggu hingga serial port terbuka
  SPI.begin();           // Inisialisasi SPI bus
  mfrc522.PCD_Init();    // Inisialisasi MFRC522
  delay(4);              // Delay opsional, beberapa board membutuhkan waktu lebih lama setelah inisialisasi
  mfrc522.PCD_DumpVersionToSerial();  // Tampilkan detail dari pembaca kartu MFRC522
  Serial.println(F("Scan PICC to write data..."));

  // Set default key
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  // Reset loop jika tidak ada kartu baru yang terdeteksi
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Pilih salah satu kartu
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Data yang akan ditulis
  byte dataLength = strlen(data);
  byte blocksRequired = (dataLength + 15) / 16;  // Hitung jumlah blok yang diperlukan

  // Menulis data ke beberapa blok berturut-turut, mulai dari blok 1
  for (byte i = 0; i < blocksRequired; i++) {
    byte block = 1 + i;  // Mulai dari blok 1

    // Otentikasi dengan blok menggunakan kunci A
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Persiapkan data untuk blok
    byte buffer[16];
    for (byte j = 0; j < 16; j++) {
      byte index = i * 16 + j;
      buffer[j] = (index < dataLength) ? data[index] : 0;
    }

    // Tulis data ke blok
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
  }

  Serial.println(F("Data has been written successfully."));

  // Berhentikan kartu agar dapat membaca kartu lainnya
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
