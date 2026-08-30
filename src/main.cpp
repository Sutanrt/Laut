#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// DEVICE
// =====================================================

#define DEVICE_ID "BOAT_01"

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// MPU6050
// =====================================================

#define MPU_ADDR 0x68

#define MPU_PWR_MGMT_1   0x6B
#define MPU_WHO_AM_I     0x75
#define MPU_ACCEL_CONFIG 0x1C
#define MPU_ACCEL_XOUT_H 0x3B

// =====================================================
// GPS
// =====================================================

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

#define GPS_RX 16
#define GPS_TX 17

// =====================================================
// LORA
// =====================================================

#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 26

#define LORA_FREQUENCY 433E6

// =====================================================
// BUZZER AKTIF
// =====================================================

#define BUZZER_PIN 25

// =====================================================
// SYSTEM
// =====================================================

float maxTiltAngle = 25.0;

unsigned long sendInterval = 2000;
unsigned long lastSendTime = 0;

unsigned long remoteTimeout = 10000;

unsigned long lastDisplay = 0;

unsigned long lastBuzzerTime = 0;
bool buzzerState = false;




int displayPage = 0;

// =====================================================
// DATA KAPAL SENDIRI
// =====================================================

float latitude = 0.0;
float longitude = 0.0;
float speedKmph = 0.0;

float angleX = 0.0;
float angleY = 0.0;
float accelZ = 0;


bool gpsValid = false;
bool danger = false;

// =====================================================
// DATA KAPAL LAIN
// =====================================================

struct RemoteBoat {

  String id;

  float lat;
  float lng;

  float speed;

  float tiltX;
  float tiltY;

  bool gpsValid;
  bool danger;

  int rssi;

  unsigned long lastSeen;
};

#define MAX_REMOTE_BOATS 5

RemoteBoat boats[MAX_REMOTE_BOATS];

// =====================================================
// FUNCTION DECLARATION
// =====================================================

bool initMPU6050();

void writeMPURegister(
  byte reg,
  byte value
);

void readGPS();

void readMPU();

void checkDanger();

void sendLoRaData();

void receiveLoRaData();

void processPacket(
  String packet,
  int rssi
);

int findBoat(
  String id
);

void updateOLED();

void alarmOwnDanger();

void alarmRemoteDanger();

void removeInactiveBoats();

void updateBuzzer();

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" BOAT 01 - MPU6050");
  Serial.println("==============================");

  // ---------------------------------------------------
  // BUZZER AKTIF
  // ---------------------------------------------------

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  // ---------------------------------------------------
  // I2C
  // ---------------------------------------------------

  Wire.begin(
    21,
    22
  );

  // ---------------------------------------------------
  // OLED
  // ---------------------------------------------------

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println(
      "OLED ERROR"
    );

    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(1);

  display.setCursor(0, 0);

  display.println(
    "BOAT 01"
  );

  display.println(
    "Starting..."
  );

  display.display();

  // ---------------------------------------------------
  // MPU6050
  // ---------------------------------------------------

  if (!initMPU6050()) {

    Serial.println(
      "MPU6050 ERROR"
    );

    display.clearDisplay();

    display.setCursor(0, 0);

    display.println(
      "MPU6050 ERROR"
    );

    display.display();

    while (true) {
      delay(100);
    }
  }

  Serial.println(
    "MPU6050 OK"
  );

  // ---------------------------------------------------
  // GPS
  // ---------------------------------------------------

  gpsSerial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX,
    GPS_TX
  );

  Serial.println(
    "GPS UART OK"
  );

  // ---------------------------------------------------
  // LORA
  // ---------------------------------------------------

  SPI.begin(
    LORA_SCK,
    LORA_MISO,
    LORA_MOSI,
    LORA_SS
  );

  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  if (!LoRa.begin(
        LORA_FREQUENCY
      )) {

    Serial.println(
      "LoRa ERROR"
    );

    display.clearDisplay();

    display.setCursor(0, 0);

    display.println(
      "LoRa ERROR"
    );

    display.display();

    while (true) {
      delay(100);
    }
  }

  // ---------------------------------------------------
  // LORA SETTINGS
  // ---------------------------------------------------

  LoRa.setTxPower(17);

  LoRa.setSpreadingFactor(7);

  LoRa.setSignalBandwidth(
    125E3
  );

  LoRa.setCodingRate4(5);

  LoRa.enableCrc();

  LoRa.setSyncWord(0x34);

  Serial.println(
    "LoRa OK"
  );

  // ---------------------------------------------------
  // READY
  // ---------------------------------------------------

  display.clearDisplay();

  display.setCursor(0, 0);

  display.println(
    "BOAT 01 READY"
  );

  display.println(
    "MPU6050 OK"
  );

  display.println(
    "LoRa OK"
  );

  display.println(
    "GPS WAIT..."
  );

  display.display();

  delay(1500);

  // Mulai receive mode
  LoRa.receive();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  readGPS();

  readMPU();

  checkDanger();

  receiveLoRaData();

  removeInactiveBoats();

  updateOLED();

  updateBuzzer();

  if (
    millis() - lastSendTime >=
    sendInterval
  ) {

    sendLoRaData();

    lastSendTime = millis();
  }
}

void removeInactiveBoats() {

  for (
    int i = 0;
    i < MAX_REMOTE_BOATS;
    i++
  ) {

    if (
      boats[i].id.length() > 0 &&
      millis() - boats[i].lastSeen >
      remoteTimeout
    ) {

      boats[i].id = "";
      boats[i].danger = false;
      boats[i].gpsValid = false;
    }
  }
}

// =====================================================
// INIT MPU6050
// =====================================================

bool initMPU6050() {

  Wire.beginTransmission(
    MPU_ADDR
  );

  if (
    Wire.endTransmission() != 0
  ) {

    return false;
  }

  Wire.beginTransmission(
    MPU_ADDR
  );

  Wire.write(
    MPU_WHO_AM_I
  );

  if (
    Wire.endTransmission(false) != 0
  ) {

    return false;
  }

  Wire.requestFrom(
    MPU_ADDR,
    1
  );

  if (!Wire.available()) {

    return false;
  }

  byte whoAmI =
    Wire.read();

  Serial.print(
    "MPU WHO_AM_I: 0x"
  );

  Serial.println(
    whoAmI,
    HEX
  );

  if (
    whoAmI != 0x68
  ) {

    Serial.println(
      "Bukan MPU6050"
    );

    return false;
  }

  writeMPURegister(
    MPU_PWR_MGMT_1,
    0x00
  );

  delay(100);

  writeMPURegister(
    MPU_ACCEL_CONFIG,
    0x10
  );

  delay(50);

  return true;
}

// =====================================================
// WRITE MPU
// =====================================================

void writeMPURegister(
  byte reg,
  byte value
) {

  Wire.beginTransmission(
    MPU_ADDR
  );

  Wire.write(reg);

  Wire.write(value);

  Wire.endTransmission();
}


void updateBuzzer() {

  bool remoteDanger = false;

  for (int i = 0; i < MAX_REMOTE_BOATS; i++) {

    if (
      boats[i].id.length() > 0 &&
      boats[i].danger
    ) {

      remoteDanger = true;
      break;
    }
  }

  // Prioritas danger sendiri
  if (danger) {

    if (
      millis() - lastBuzzerTime >= 150
    ) {

      lastBuzzerTime = millis();

      buzzerState = !buzzerState;

      digitalWrite(
        BUZZER_PIN,
        buzzerState
      );
    }

    return;
  }

  // Danger kapal lain
  if (remoteDanger) {

    if (
      millis() - lastBuzzerTime >= 400
    ) {

      lastBuzzerTime = millis();

      buzzerState = !buzzerState;

      digitalWrite(
        BUZZER_PIN,
        buzzerState
      );
    }

    return;
  }

  // Tidak ada danger
  buzzerState = false;

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}

// =====================================================
// GPS
// =====================================================

void readGPS() {

  while (
    gpsSerial.available()
  ) {

    gps.encode(
      gpsSerial.read()
    );
  }

  if (
    gps.location.isValid()
  ) {

    gpsValid = true;

    latitude =
      gps.location.lat();

    longitude =
      gps.location.lng();

  } else {

    gpsValid = false;
  }

  if (
    gps.speed.isValid()
  ) {

    speedKmph =
      gps.speed.kmph();
  }
}

// =====================================================
// MPU6050
// =====================================================

void readMPU() {

  Wire.beginTransmission(
    MPU_ADDR
  );

  Wire.write(
    MPU_ACCEL_XOUT_H
  );

  if (
    Wire.endTransmission(false) != 0
  ) {

    return;
  }

  Wire.requestFrom(
    MPU_ADDR,
    6,
    true
  );

  if (
    Wire.available() < 6
  ) {

    return;
  }

  int16_t rawAx =
    ((int16_t)Wire.read() << 8)
    | Wire.read();

  int16_t rawAy =
    ((int16_t)Wire.read() << 8)
    | Wire.read();

  int16_t rawAz =
    ((int16_t)Wire.read() << 8)
    | Wire.read();

  float ax =
    rawAx / 4096.0;

  float ay =
    rawAy / 4096.0;

  float az =
    rawAz / 4096.0;

  angleX =
    atan2(
      ay,
      sqrt(
        ax * ax +
        az * az
      )
    )
    * 180.0 /
    PI;

  angleY =
    atan2(
      -ax,
      sqrt(
        ay * ay +
        az * az
      )
    )
    * 180.0 /
    PI;

  accelZ = az;
}

// =====================================================
// CHECK DANGER
// =====================================================

void checkDanger() {

  danger = false;

  if (
    fabs(angleX) > maxTiltAngle ||
    fabs(angleY) > maxTiltAngle ||
    angleX < -maxTiltAngle ||
    angleY < -maxTiltAngle
  ) {

    danger = true;
  }

  if (accelZ < 0) {
    danger = true;
}
}

// =====================================================
// SEND LORA
// =====================================================

void sendLoRaData() {

  String packet = "";

  packet += "ID=";
  packet += DEVICE_ID;

  packet += ",STATUS=";

  packet += (
    danger
      ? "DANGER"
      : "SAFE"
  );

  packet += ",GPS=";

  packet += (
    gpsValid
      ? "1"
      : "0"
  );

  packet += ",LAT=";
  packet += String(
    latitude,
    6
  );

  packet += ",LNG=";
  packet += String(
    longitude,
    6
  );

  packet += ",SPEED=";
  packet += String(
    speedKmph,
    2
  );

  packet += ",TX=";
  packet += String(
    angleX,
    1
  );

  packet += ",TY=";
  packet += String(
    angleY,
    1
  );

  Serial.print(
    "TX: "
  );

  Serial.println(
    packet
  );

  LoRa.idle();

  LoRa.beginPacket();

  LoRa.print(packet);

  LoRa.endPacket();

  LoRa.receive();
}

// =====================================================
// RECEIVE LORA
// =====================================================

void receiveLoRaData() {

  int packetSize =
    LoRa.parsePacket();

  if (
    packetSize <= 0
  ) {

    return;
  }

  String packet = "";

  while (
    LoRa.available()
  ) {

    packet +=
      (char)LoRa.read();
  }

  int rssi =
    LoRa.packetRssi();

  Serial.print(
    "RX: "
  );

  Serial.println(
    packet
  );

  processPacket(
    packet,
    rssi
  );

  LoRa.receive();
}

// =====================================================
// FIND BOAT
// =====================================================

int findBoat(
  String id
) {

  for (
    int i = 0;
    i < MAX_REMOTE_BOATS;
    i++
  ) {

    if (
      boats[i].id == id
    ) {

      return i;
    }
  }

  for (
    int i = 0;
    i < MAX_REMOTE_BOATS;
    i++
  ) {

    if (
      boats[i].id.length() == 0
    ) {

      boats[i].id = id;

      return i;
    }
  }

  return -1;
}

// =====================================================
// GET VALUE FROM PACKET
// =====================================================

String getValue(
  String packet,
  String key
) {

  String target =
    key + "=";

  int start =
    packet.indexOf(target);

  if (
    start < 0
  ) {

    return "";
  }

  start +=
    target.length();

  int end =
    packet.indexOf(
      ",",
      start
    );

  if (
    end < 0
  ) {

    end =
      packet.length();
  }

  return packet.substring(
    start,
    end
  );
}

// =====================================================
// PROCESS PACKET
// =====================================================

void processPacket(
  String packet,
  int rssi
) {

  String id =
    getValue(
      packet,
      "ID"
    );

  if (
    id.length() == 0
  ) {

    return;
  }

  // Jangan masukkan kapal sendiri
  if (
    id == DEVICE_ID
  ) {

    return;
  }

  int index =
    findBoat(id);

  if (
    index < 0
  ) {

    return;
  }

  boats[index].id = id;

  String status =
    getValue(
      packet,
      "STATUS"
    );

  boats[index].danger =
    (
      status == "DANGER"
    );

  boats[index].gpsValid =
    (
      getValue(
        packet,
        "GPS"
      ) == "1"
    );

  boats[index].lat =
    getValue(
      packet,
      "LAT"
    ).toFloat();

  boats[index].lng =
    getValue(
      packet,
      "LNG"
    ).toFloat();

  boats[index].speed =
    getValue(
      packet,
      "SPEED"
    ).toFloat();

  boats[index].tiltX =
    getValue(
      packet,
      "TX"
    ).toFloat();

  boats[index].tiltY =
    getValue(
      packet,
      "TY"
    ).toFloat();

  boats[index].rssi =
    rssi;

  boats[index].lastSeen =
    millis();
}

// =====================================================
// OLED
// =====================================================

void updateOLED() {

  display.clearDisplay();

  display.setTextSize(1);

  // ---------------------------------------------------
  // Cari kapal aktif
  // ---------------------------------------------------

  int count = 0;

  int dangerIndex = -1;

  for (
    int i = 0;
    i < MAX_REMOTE_BOATS;
    i++
  ) {

    if (
      boats[i].id.length() > 0
    ) {

      count++;

      if (
        boats[i].danger
      ) {

        dangerIndex = i;
      }
    }
  }

  // ---------------------------------------------------
  // Jika ada kapal DANGER
  // ---------------------------------------------------

  if (
    dangerIndex >= 0
  ) {

    RemoteBoat &b =
      boats[dangerIndex];

    display.setCursor(0, 0);

    display.println(
      "!!! DANGER !!!"
    );

    display.print(
      "Kapal: "
    );

    display.println(
      b.id
    );

    display.print(
      "Lat:"
    );

    display.println(
      b.lat,
      6
    );

    display.print(
      "Lng:"
    );

    display.println(
      b.lng,
      6
    );

    display.print(
      "Tilt:"
    );

    display.print(
      b.tiltX,
      1
    );

    display.print(
      "/"
    );

    display.println(
      b.tiltY,
      1
    );

    display.print(
      "RSSI:"
    );

    display.println(
      b.rssi
    );

    display.display();

    return;
  }

  // ---------------------------------------------------
  // Normal
  // ---------------------------------------------------

  display.setCursor(0, 0);

  display.print(
    DEVICE_ID
  );

  display.println(
    " ACTIVE"
  );

  display.print(
    "GPS:"
  );

  display.println(
    gpsValid
      ? "OK"
      : "NO FIX"
  );

  display.print(
    "Kapal terdeteksi:"
  );

  display.println(
    count
  );

  display.print(
    "Tilt X:"
  );

  display.println(
    angleX,
    1
  );

  display.print(
    "Tilt Y:"
  );

  display.println(
    angleY,
    1
  );

  display.print(
    "Status:"
  );

  display.println(
    danger
      ? "DANGER"
      : "SAFE"
  );

  display.display();
}

// =====================================================
// BUZZER AKTIF
// =====================================================

void alarmOwnDanger() {

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  delay(100);

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  delay(100);
}

// =====================================================
// REMOTE DANGER
// =====================================================

void alarmRemoteDanger() {

  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  delay(300);

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  delay(300);
}