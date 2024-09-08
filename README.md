# Wemos RFID Write

This project allows you to write data to Mifare RFID cards using the Wemos D1 Mini (ESP8266) and the RC522 RFID module. It’s a simple, yet effective solution for applications such as access control, inventory management, and smart systems that require storing information on RFID cards.

## Features
- Write and store data on RFID cards using the RC522 module
- Easy-to-use interface with clear code and comments
- Works seamlessly with the Wemos D1 Mini (ESP8266)
- Communication via SPI protocol between Wemos and RFID module
- Well-documented, beginner-friendly project

## Getting Started

### Requirements
- **Hardware**:
  - Wemos D1 Mini (ESP8266)
  - RC522 RFID Reader/Writer module
  - Mifare RFID cards
  - Jumper wires

- **Software**:
  - Arduino IDE
  - Required libraries: 
    - [MFRC522](https://github.com/miguelbalboa/rfid)
    - ESP8266 board support for Arduino

### Circuit Diagram
Connect the RC522 RFID module to the Wemos D1 Mini as follows:
- **SDA** → D1
- **SCK** → D5
- **MOSI** → D7
- **MISO** → D6
- **IRQ** → Not connected
- **GND** → GND
- **RST** → D2
- **VCC** → 3.3V

### Installation

1. Clone this repository:
    ```bash
    git clone https://github.com/ExiroStudio/wemos-rfid-write.git
    ```

2. Install the required libraries in the Arduino IDE:
    - Search and install the **MFRC522** library from the Library Manager.
    - Install **ESP8266** board support in the Board Manager.

3. Open the Arduino IDE and load the provided sketch from this repository.

4. Compile and upload the code to your Wemos D1 Mini.

### How to Use

1. After uploading the sketch, open the Serial Monitor (baud rate: 115200).
2. Place an RFID card near the RC522 module.
3. The system will write data to the card based on the code logic, and the Serial Monitor will show the status of the write operation.

## Code Example

Here’s a basic example of writing data to an RFID card:
```cpp
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D2  // Pin untuk Reset
#define SS_PIN D1   // Pin untuk Slave Select (SDA)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Buat instance MFRC522

// Default key untuk otentikasi
MFRC522::MIFARE_Key key;

const char* data = "Your Data";

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

```

## Contributing

If you want to contribute to this project, feel free to fork the repository and submit pull requests. Contributions are always welcome!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
