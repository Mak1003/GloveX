#include <Wire.h>
#include <BluetoothSerial.h>

// ================== OBJECTS ==================
BluetoothSerial SerialBT;

// ================== PIN DEFINITIONS ==================
#define FLEX1_PIN 34  // Thumb
#define FLEX2_PIN 35  // Index
#define FLEX3_PIN 32  // Middle
#define FLEX4_PIN 33  // Ring
#define FLEX5_PIN 25  // Little finger
#define LED_PIN 2     // Status LED
#define MPU_ADDR 0x68 // MPU6050 I2C Address

// ================== CALIBRATION RANGES ==================
int flexMin[5] = {150, 150, 150, 150, 150};
int flexMax[5] = {1500, 1500, 1500, 1500, 1500};
int flexPercent[5];
String gesture = "";

// MPU variables
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
float pitch = 0, roll = 0, yaw = 0;

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA, SCL for ESP32
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("\n===== GloveX+ Real-World Sign Translator =====");

  // Bluetooth init
  if (!SerialBT.begin("GloveX_PLUS")) {
    Serial.println("❌ Bluetooth Init Failed!");
  } else {
    Serial.println("✅ Bluetooth Ready as 'GloveX_PLUS'");
  }

  // MPU Init
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00); // Wake up MPU6050
  if (Wire.endTransmission() == 0) {
    Serial.println("✅ MPU6050 Initialized Successfully!");
  } else {
    Serial.println("⚠️ MPU6050 Not Detected! Continuing with flex sensors only...");
  }

  Serial.println("System Ready ✅");
  Serial.println("Tip: Keep glove straight during startup for calibration.\n");
}

// ================== MAIN LOOP ==================
void loop() {
  readFlexSensors();
  readMPU();
  detectRealWorldGesture();
  delay(500);
}

// ================== FLEX SENSOR READING ==================
void readFlexSensors() {
  int flexPins[5] = {FLEX1_PIN, FLEX2_PIN, FLEX3_PIN, FLEX4_PIN, FLEX5_PIN};
  Serial.print("Flex %: ");

  for (int i = 0; i < 5; i++) {
    int raw = analogRead(flexPins[i]);
    flexPercent[i] = map(raw, flexMin[i], flexMax[i], 0, 100);
    flexPercent[i] = constrain(flexPercent[i], 0, 100);
    Serial.print(flexPercent[i]);
    Serial.print("% ");
  }
  Serial.println();
}

// ================== MPU READING ==================
void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  if (Wire.available() == 14) {
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); // skip temperature
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();

    pitch = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180.0 / PI;
    roll  = atan2(-AcX, AcZ) * 180.0 / PI;
    yaw   += GyZ / 131.0 * 0.01;

    Serial.print("MPU => Pitch:");
    Serial.print(pitch);
    Serial.print(" Roll:");
    Serial.print(roll);
    Serial.print(" Yaw:");
    Serial.println(yaw);
  }
}

// ================== REAL-WORLD GESTURE DETECTION ==================
void detectRealWorldGesture() {
  String flexPattern = "";
  for (int i = 0; i < 5; i++) {
    flexPattern += (flexPercent[i] >= 60) ? "1" : "0";
  }

  gesture = "";
  Serial.print("Pattern: "); Serial.println(flexPattern);

  // ===================== REAL SIGN LOGIC =====================

  // 🤚 NEUTRAL / OPEN HAND
  if (flexPattern == "00000" && abs(pitch) < 20 && abs(roll) < 20)
    gesture = "NEUTRAL";

  // ✊ FIST
  else if (flexPattern == "11111")
    gesture = "FIST (A / YES)";

  // 👍 THUMBS UP
  else if (flexPattern == "01111" && roll < -20)
    gesture = "THUMBS UP (GOOD / OK)";

  // 👎 THUMBS DOWN
  else if (flexPattern == "01111" && roll > 20)
    gesture = "THUMBS DOWN (BAD / NO)";

  // 👋 HELLO / BYE
  else if (flexPattern == "00000" && abs(yaw) > 40)
    gesture = "HELLO / BYE";

  // 🤟 I LOVE YOU (thumb, index, pinky open)
  else if (flexPattern == "10001")
    gesture = "I LOVE YOU";

  // 🖐 STOP (all fingers open, palm facing front)
  else if (flexPattern == "00000" && pitch > 30)
    gesture = "STOP";

  // 🙏 PLEASE / THANK YOU (fingers bent slightly)
  else if (flexPattern == "01110" && abs(roll) < 20)
    gesture = "THANK YOU / PLEASE";

  // ✋ HELP (flat hand, tilt up)
  else if (flexPattern == "00000" && pitch > 45)
    gesture = "HELP";

  // 👆 UP / DOWN
  else if (flexPattern == "10000" && pitch > 40)
    gesture = "MOVE UP";
  else if (flexPattern == "10000" && pitch < -40)
    gesture = "MOVE DOWN";

  // 👉 / 👈 DIRECTION
  else if (flexPattern == "10000" && roll > 40)
    gesture = "MOVE LEFT";
  else if (flexPattern == "10000" && roll < -40)
    gesture = "MOVE RIGHT";

  // 🤜🤛 YES / NO MOTION
  else if (flexPattern == "11111" && abs(yaw) > 30)
    gesture = "YES (NOD)";
  else if (flexPattern == "11111" && abs(pitch) > 30)
    gesture = "NO (SHAKE)";

  // 🤏 SMALL / LITTLE
  else if (flexPattern == "11110")
    gesture = "SMALL / LITTLE";

  // 👌 OK SIGN
  else if (flexPattern == "10011")
    gesture = "OK";

  // 🤚 WAIT
  else if (flexPattern == "00000" && pitch < -30)
    gesture = "WAIT";

  // ✋ EMERGENCY (fist + fast shake)
  else if (flexPattern == "11111" && abs(GyX) > 500)
    gesture = "EMERGENCY";

  // 🫱 COME HERE
  else if (flexPattern == "00000" && roll > 30 && GyY > 100)
    gesture = "COME HERE";

  // 🫲 GO AWAY
  else if (flexPattern == "00000" && roll < -30 && GyY < -100)
    gesture = "GO AWAY";

  // ✌️ PEACE
  else if (flexPattern == "00100")
    gesture = "PEACE";

  // 🖖 LIVE LONG
  else if (flexPattern == "10101")
    gesture = "LIVE LONG";

  // ===================== OUTPUT =====================
  if (gesture != "") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("🖐 Gesture: " + gesture);
    SerialBT.println(gesture); // Send to Bluetooth app
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
