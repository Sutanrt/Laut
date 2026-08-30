// #include <Wire.h>
// #include <SPI.h>
// #include <LoRa.h>
// #include <TinyGPSPlus.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// // =====================================================
// // DEVICE
// // =====================================================

// #define DEVICE_ID "BOAT_02"

// // =====================================================
// // OLED
// // =====================================================

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_RESET -1

// Adafruit_SSD1306 display(
//   SCREEN_WIDTH,
//   SCREEN_HEIGHT,
//   &Wire,
//   OLED_RESET
// );

// // =====================================================
// // MPU6500
// // =====================================================

// #define MPU_ADDR 0x68

// #define MPU_PWR_MGMT_1   0x6B
// #define MPU_WHO_AM_I     0x75
// #define MPU_ACCEL_CONFIG 0x1C
// #define MPU_ACCEL_XOUT_H 0x3B

// // =====================================================
// // GPS
// // =====================================================

// TinyGPSPlus gps;
// HardwareSerial gpsSerial(2);

// #define GPS_RX 16
// #define GPS_TX 17

// // =====================================================
// // LORA
// // =====================================================

// #define LORA_SCK  18
// #define LORA_MISO 19
// #define LORA_MOSI 23
// #define LORA_SS   5
// #define LORA_RST  14
// #define LORA_DIO0 26

// #define LORA_FREQUENCY 433E6

// // =====================================================
// // BUZZER PASIF
// // =====================================================

// #define BUZZER_PIN 25

// // =====================================================
// // SYSTEM
// // =====================================================

// float maxTiltAngle = 25.0;

// unsigned long sendInterval = 2000;
// unsigned long nextSendTime = 0;

// unsigned long lastSendTime = 0;

// unsigned long remoteTimeout = 10000;

// unsigned long lastDisplay = 0;


// unsigned long lastBuzzerTime = 0;
// bool buzzerState = false;

// // =====================================================
// // DATA SENDIRI
// // =====================================================

// float latitude = 0.0;
// float longitude = 0.0;
// float speedKmph = 0.0;

// float angleX = 0.0;
// float angleY = 0.0;
// float accelZ = 0;

// bool gpsValid = false;
// bool danger = false;

// // =====================================================
// // DATA KAPAL LAIN
// // =====================================================

// struct RemoteBoat {

//   String id;

//   float lat;
//   float lng;

//   float speed;

//   float tiltX;
//   float tiltY;

//   bool gpsValid;
//   bool danger;

//   int rssi;

//   unsigned long lastSeen;
// };

// #define MAX_REMOTE_BOATS 5

// RemoteBoat boats[MAX_REMOTE_BOATS];

// // =====================================================
// // FUNCTION DECLARATION
// // =====================================================

// bool initMPU6500();

// void writeMPURegister(
//   byte reg,
//   byte value
// );

// void readGPS();

// void readMPU();

// void checkDanger();

// void sendLoRaData();

// void receiveLoRaData();

// void updateBuzzer();

// void removeInactiveBoats();


// void processPacket(
//   String packet,
//   int rssi
// );

// int findBoat(
//   String id
// );

// String getValue(
//   String packet,
//   String key
// );

// void updateOLED();

// void alarmOwnDanger();

// void alarmRemoteDanger();

// // =====================================================
// // SETUP
// // =====================================================

// void setup() {

//   Serial.begin(115200);

//   delay(1000);

//   Serial.println();
//   Serial.println("==============================");
//   Serial.println(" BOAT 02 - MPU6500");
//   Serial.println("==============================");

//   //LORA
//   randomSeed(analogRead(34));

//   nextSendTime = millis() + random(500, 1500);

//   // ---------------------------------------------------
//   // BUZZER PASIF
//   // ---------------------------------------------------

//   pinMode(
//     BUZZER_PIN,
//     OUTPUT
//   );

//   noTone(
//     BUZZER_PIN
//   );

//   // ---------------------------------------------------
//   // I2C
//   // ---------------------------------------------------

//   Wire.begin(
//     21,
//     22
//   );

//   // ---------------------------------------------------
//   // OLED
//   // ---------------------------------------------------

//   if (!display.begin(
//         SSD1306_SWITCHCAPVCC,
//         0x3C
//       )) {

//     Serial.println(
//       "OLED ERROR"
//     );

//     while (true) {
//       delay(100);
//     }
//   }

//   display.clearDisplay();

//   display.setTextColor(
//     SSD1306_WHITE
//   );

//   display.setTextSize(1);

//   display.setCursor(0, 0);

//   display.println(
//     "BOAT 02"
//   );

//   display.println(
//     "Starting..."
//   );

//   display.display();

//   // ---------------------------------------------------
//   // MPU6500
//   // ---------------------------------------------------

//   if (!initMPU6500()) {

//     Serial.println(
//       "MPU6500 ERROR"
//     );

//     display.clearDisplay();

//     display.setCursor(0, 0);

//     display.println(
//       "MPU6500 ERROR"
//     );

//     display.display();

//     while (true) {
//       delay(100);
//     }
//   }

//   Serial.println(
//     "MPU6500 OK"
//   );

//   // ---------------------------------------------------
//   // GPS
//   // ---------------------------------------------------

//   gpsSerial.begin(
//     9600,
//     SERIAL_8N1,
//     GPS_RX,
//     GPS_TX
//   );

//   Serial.println(
//     "GPS UART OK"
//   );

//   // ---------------------------------------------------
//   // LORA
//   // ---------------------------------------------------

//   SPI.begin(
//     LORA_SCK,
//     LORA_MISO,
//     LORA_MOSI,
//     LORA_SS
//   );

//   LoRa.setPins(
//     LORA_SS,
//     LORA_RST,
//     LORA_DIO0
//   );

//   if (!LoRa.begin(
//         LORA_FREQUENCY
//       )) {

//     Serial.println(
//       "LoRa ERROR"
//     );

//     display.clearDisplay();

//     display.setCursor(0, 0);

//     display.println(
//       "LoRa ERROR"
//     );

//     display.display();

//     while (true) {
//       delay(100);
//     }
//   }

//   // ---------------------------------------------------
//   // LORA SETTINGS
//   // ---------------------------------------------------

//   LoRa.setTxPower(17);

//   LoRa.setSpreadingFactor(7);

//   LoRa.setSignalBandwidth(
//     125E3
//   );

//   LoRa.setCodingRate4(5);

//   LoRa.enableCrc();

//   LoRa.setSyncWord(0x34);

//   Serial.println(
//     "LoRa OK"
//   );

//   // ---------------------------------------------------
//   // READY
//   // ---------------------------------------------------

//   display.clearDisplay();

//   display.setCursor(0, 0);

//   display.println(
//     "BOAT 02 READY"
//   );

//   display.println(
//     "MPU6500 OK"
//   );

//   display.println(
//     "LoRa OK"
//   );

//   display.println(
//     "GPS WAIT..."
//   );

//   display.display();

//   delay(1500);

//   LoRa.receive();
// }


// void removeInactiveBoats() {

//   for (
//     int i = 0;
//     i < MAX_REMOTE_BOATS;
//     i++
//   ) {

//     if (
//       boats[i].id.length() > 0 &&
//       millis() - boats[i].lastSeen >
//       remoteTimeout
//     ) {

//       boats[i].id = "";
//       boats[i].danger = false;
//       boats[i].gpsValid = false;
//     }
//   }
// }

// // =====================================================
// // LOOP
// // =====================================================
// void loop() {

//   readGPS();

//   readMPU();

//   checkDanger();

//   receiveLoRaData();

//   removeInactiveBoats();

//   updateOLED();

//   updateBuzzer();

//   if (millis() >= nextSendTime) {

//     sendLoRaData();

//     nextSendTime =
//         millis() + 2000 + random(0, 1000);
//   }

// }


// void updateBuzzer() {

//   bool remoteDanger = false;

//   for (int i = 0; i < MAX_REMOTE_BOATS; i++) {

//     if (
//       boats[i].id.length() > 0 &&
//       boats[i].danger
//     ) {

//       remoteDanger = true;
//       break;
//     }
//   }

//   // -----------------------------------------
//   // DANGER SENDIRI
//   // -----------------------------------------

//   if (danger) {

//     if (
//       millis() - lastBuzzerTime >= 200
//     ) {

//       lastBuzzerTime = millis();

//       if (buzzerState) {

//         noTone(
//           BUZZER_PIN
//         );

//         buzzerState = false;

//       } else {

//         tone(
//           BUZZER_PIN,
//           2500
//         );

//         buzzerState = true;
//       }
//     }

//     return;
//   }

//   // -----------------------------------------
//   // DANGER KAPAL LAIN
//   // -----------------------------------------

//   if (remoteDanger) {

//     if (
//       millis() - lastBuzzerTime >= 500
//     ) {

//       lastBuzzerTime = millis();

//       if (buzzerState) {

//         noTone(
//           BUZZER_PIN
//         );

//         buzzerState = false;

//       } else {

//         tone(
//           BUZZER_PIN,
//           1500
//         );

//         buzzerState = true;
//       }
//     }

//     return;
//   }

//   // -----------------------------------------
//   // SAFE
//   // -----------------------------------------

//   noTone(
//     BUZZER_PIN
//   );

//   buzzerState = false;
// }
// // =====================================================
// // INIT MPU6500
// // =====================================================

// bool initMPU6500() {

//   Wire.beginTransmission(
//     MPU_ADDR
//   );

//   if (
//     Wire.endTransmission() != 0
//   ) {

//     return false;
//   }

//   Wire.beginTransmission(
//     MPU_ADDR
//   );

//   Wire.write(
//     MPU_WHO_AM_I
//   );

//   if (
//     Wire.endTransmission(false) != 0
//   ) {

//     return false;
//   }

//   Wire.requestFrom(
//     MPU_ADDR,
//     1
//   );

//   if (!Wire.available()) {

//     return false;
//   }

//   byte whoAmI =
//     Wire.read();

//   Serial.print(
//     "MPU WHO_AM_I: 0x"
//   );

//   Serial.println(
//     whoAmI,
//     HEX
//   );

//   if (
//     whoAmI != 0x70
//   ) {

//     Serial.println(
//       "Bukan MPU6500"
//     );

//     return false;
//   }

//   writeMPURegister(
//     MPU_PWR_MGMT_1,
//     0x00
//   );

//   delay(100);

//   writeMPURegister(
//     MPU_ACCEL_CONFIG,
//     0x10
//   );

//   delay(50);

//   return true;
// }

// // =====================================================
// // WRITE MPU
// // =====================================================

// void writeMPURegister(
//   byte reg,
//   byte value
// ) {

//   Wire.beginTransmission(
//     MPU_ADDR
//   );

//   Wire.write(reg);

//   Wire.write(value);

//   Wire.endTransmission();
// }

// // =====================================================
// // GPS
// // =====================================================

// void readGPS() {

//   while (
//     gpsSerial.available()
//   ) {

//     gps.encode(
//       gpsSerial.read()
//     );
//   }

//   if (
//     gps.location.isValid()
//   ) {

//     gpsValid = true;

//     latitude =
//       gps.location.lat();

//     longitude =
//       gps.location.lng();

//   } else {

//     gpsValid = false;
//   }

//   if (
//     gps.speed.isValid()
//   ) {

//     speedKmph =
//       gps.speed.kmph();
//   }
// }

// // =====================================================
// // MPU6500
// // =====================================================

// void readMPU() {

//   Wire.beginTransmission(
//     MPU_ADDR
//   );

//   Wire.write(
//     MPU_ACCEL_XOUT_H
//   );

//   if (
//     Wire.endTransmission(false) != 0
//   ) {

//     return;
//   }

//   Wire.requestFrom(
//     MPU_ADDR,
//     6,
//     true
//   );

//   if (
//     Wire.available() < 6
//   ) {

//     return;
//   }

//   int16_t rawAx =
//     ((int16_t)Wire.read() << 8)
//     | Wire.read();

//   int16_t rawAy =
//     ((int16_t)Wire.read() << 8)
//     | Wire.read();

//   int16_t rawAz =
//     ((int16_t)Wire.read() << 8)
//     | Wire.read();

//   float ax =
//     rawAx / 4096.0;

//   float ay =
//     rawAy / 4096.0;

//   float az =
//     rawAz / 4096.0;

//   angleX =
//     atan2(
//       ay,
//       sqrt(
//         ax * ax +
//         az * az
//       )
//     )
//     * 180.0 /
//     PI;

//   angleY =
//     atan2(
//       -ax,
//       sqrt(
//         ay * ay +
//         az * az
//       )
//     )
//     * 180.0 /
//     PI;

//     accelZ=az;
// }

// // =====================================================
// // CHECK DANGER
// // =====================================================

// void checkDanger() {

//   danger = false;

//   if (
//     fabs(angleX) > maxTiltAngle ||
//     fabs(angleY) > maxTiltAngle ||
//     angleX < -maxTiltAngle ||
//     angleY < -maxTiltAngle
//   ) {

//     danger = true;
//   }

//   if (accelZ < 0) {
//     danger = true;
// }
// }

// // =====================================================
// // SEND LORA
// // =====================================================

// void sendLoRaData() {

//   String packet = "";

//   packet += "ID=";
//   packet += DEVICE_ID;

//   packet += ",STATUS=";

//   packet += (
//     danger
//       ? "DANGER"
//       : "SAFE"
//   );

//   packet += ",GPS=";

//   packet += (
//     gpsValid
//       ? "1"
//       : "0"
//   );

//   packet += ",LAT=";

//   packet += String(
//     latitude,
//     6
//   );

//   packet += ",LNG=";

//   packet += String(
//     longitude,
//     6
//   );

//   packet += ",SPEED=";

//   packet += String(
//     speedKmph,
//     2
//   );

//   packet += ",TX=";

//   packet += String(
//     angleX,
//     1
//   );

//   packet += ",TY=";

//   packet += String(
//     angleY,
//     1
//   );

//   Serial.print(
//     "TX: "
//   );

//   Serial.println(
//     packet
//   );

//   LoRa.idle();

//   LoRa.beginPacket();

//   LoRa.print(
//     packet
//   );

//   LoRa.endPacket();

//   LoRa.receive();
// }

// // =====================================================
// // RECEIVE LORA
// // =====================================================

// void receiveLoRaData() {

//   int packetSize =
//     LoRa.parsePacket();

//   if (
//     packetSize <= 0
//   ) {

//     return;
//   }

//   String packet = "";

//   while (
//     LoRa.available()
//   ) {

//     packet +=
//       (char)LoRa.read();
//   }

//   int rssi =
//     LoRa.packetRssi();

//   Serial.print(
//     "RX: "
//   );

//   Serial.println(
//     packet
//   );

//   processPacket(
//     packet,
//     rssi
//   );

//   LoRa.receive();
// }

// // =====================================================
// // FIND BOAT
// // =====================================================

// int findBoat(
//   String id
// ) {

//   for (
//     int i = 0;
//     i < MAX_REMOTE_BOATS;
//     i++
//   ) {

//     if (
//       boats[i].id == id
//     ) {

//       return i;
//     }
//   }

//   for (
//     int i = 0;
//     i < MAX_REMOTE_BOATS;
//     i++
//   ) {

//     if (
//       boats[i].id.length() == 0
//     ) {

//       boats[i].id = id;

//       return i;
//     }
//   }

//   return -1;
// }

// // =====================================================
// // GET VALUE
// // =====================================================

// String getValue(
//   String packet,
//   String key
// ) {

//   String target =
//     key + "=";

//   int start =
//     packet.indexOf(
//       target
//     );

//   if (
//     start < 0
//   ) {

//     return "";
//   }

//   start +=
//     target.length();

//   int end =
//     packet.indexOf(
//       ",",
//       start
//     );

//   if (
//     end < 0
//   ) {

//     end =
//       packet.length();
//   }

//   return packet.substring(
//     start,
//     end
//   );
// }

// // =====================================================
// // PROCESS PACKET
// // =====================================================

// void processPacket(
//   String packet,
//   int rssi
// ) {

//   String id =
//     getValue(
//       packet,
//       "ID"
//     );

//   if (
//     id.length() == 0
//   ) {

//     return;
//   }

//   // Jangan baca paket sendiri
//   if (
//     id == DEVICE_ID
//   ) {

//     return;
//   }

//   int index =
//     findBoat(id);

//   if (
//     index < 0
//   ) {

//     return;
//   }

//   boats[index].id =
//     id;

//   String status =
//     getValue(
//       packet,
//       "STATUS"
//     );

//   boats[index].danger =
//     (
//       status == "DANGER"
//     );

//   boats[index].gpsValid =
//     (
//       getValue(
//         packet,
//         "GPS"
//       ) == "1"
//     );

//   boats[index].lat =
//     getValue(
//       packet,
//       "LAT"
//     ).toFloat();

//   boats[index].lng =
//     getValue(
//       packet,
//       "LNG"
//     ).toFloat();

//   boats[index].speed =
//     getValue(
//       packet,
//       "SPEED"
//     ).toFloat();

//   boats[index].tiltX =
//     getValue(
//       packet,
//       "TX"
//     ).toFloat();

//   boats[index].tiltY =
//     getValue(
//       packet,
//       "TY"
//     ).toFloat();

//   boats[index].rssi =
//     rssi;

//   boats[index].lastSeen =
//     millis();
// }

// // =====================================================
// // OLED
// // =====================================================

// void updateOLED() {

//   display.clearDisplay();

//   display.setTextSize(1);

//   int count = 0;

//   int dangerIndex = -1;

//   for (
//     int i = 0;
//     i < MAX_REMOTE_BOATS;
//     i++
//   ) {

//     if (
//       boats[i].id.length() > 0
//     ) {

//       count++;

//       if (
//         boats[i].danger
//       ) {

//         dangerIndex = i;
//       }
//     }
//   }

//   // ---------------------------------------------------
//   // DANGER KAPAL LAIN
//   // ---------------------------------------------------

//   if (
//     dangerIndex >= 0
//   ) {

//     RemoteBoat &b =
//       boats[dangerIndex];

//     display.setCursor(
//       0,
//       0
//     );

//     display.println(
//       "!!! DANGER !!!"
//     );

//     display.print(
//       "Kapal: "
//     );

//     display.println(
//       b.id
//     );

//     display.print(
//       "Lat:"
//     );

//     display.println(
//       b.lat,
//       6
//     );

//     display.print(
//       "Lng:"
//     );

//     display.println(
//       b.lng,
//       6
//     );

//     display.print(
//       "Tilt:"
//     );

//     display.print(
//       b.tiltX,
//       1
//     );

//     display.print(
//       "/"
//     );

//     display.println(
//       b.tiltY,
//       1
//     );

//     display.print(
//       "RSSI:"
//     );

//     display.println(
//       b.rssi
//     );

//     display.display();

//     return;
//   }

//   // ---------------------------------------------------
//   // NORMAL
//   // ---------------------------------------------------

//   display.setCursor(
//     0,
//     0
//   );

//   display.print(
//     DEVICE_ID
//   );

//   display.println(
//     " ACTIVE"
//   );

//   display.print(
//     "GPS:"
//   );

//   display.println(
//     gpsValid
//       ? "OK"
//       : "NO FIX"
//   );

//   display.print(
//     "Kapal terdeteksi:"
//   );

//   display.println(
//     count
//   );

//   display.print(
//     "Tilt X:"
//   );

//   display.println(
//     angleX,
//     1
//   );

//   display.print(
//     "Tilt Y:"
//   );

//   display.println(
//     angleY,
//     1
//   );

//   display.print(
//     "Status:"
//   );

//   display.println(
//     danger
//       ? "DANGER"
//       : "SAFE"
//   );

//   display.display();
// }

// // =====================================================
// // BUZZER PASIF - OWN DANGER
// // =====================================================

// void alarmOwnDanger() {

//   tone(
//     BUZZER_PIN,
//     2500
//   );

//   delay(150);

//   noTone(
//     BUZZER_PIN
//   );

//   delay(100);
// }

// // =====================================================
// // BUZZER PASIF - REMOTE DANGER
// // =====================================================

// void alarmRemoteDanger() {

//   tone(
//     BUZZER_PIN,
//     1500
//   );

//   delay(400);

//   noTone(
//     BUZZER_PIN
//   );

//   delay(250);
// }