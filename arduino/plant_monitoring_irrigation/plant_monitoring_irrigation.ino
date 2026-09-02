#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Bluetooth Module
SoftwareSerial BTSerial(10, 11); // RX, TX

// Pin Definitions
#define FLOAT_SWITCH_PIN 7  // Water level sensor
#define BUZZER_PIN 9        // Buzzer for alerts
#define LED_PIN 13          // Water level indicator LED
#define SOIL_SENSOR_PIN A0  // Soil moisture sensor
#define PUMP_PIN 8          // Pump relay
#define PH_SENSOR_PIN A1    // pH Sensor

// Moisture & pH Calibration
#define MOISTURE_THRESHOLD 5
float phOffset = 4.3;

// State Variables
bool pumpState = false;
bool manualControl = false;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  
  pinMode(FLOAT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  // Ensure pump is OFF at startup
  digitalWrite(PUMP_PIN, HIGH);
  pumpState = false;
  
  lcd.setCursor(0, 0);
  lcd.print("System Start...");
  delay(2000);
  lcd.clear();
  
  Serial.println("Bluetooth Ready!");
  BTSerial.println("Bluetooth Connected!");
}

void loop() {
  // === WATER LEVEL SENSOR ===
  // LOW = water available (float switch closed)
  // HIGH = no water (float switch open)
  bool waterAvailable = (digitalRead(FLOAT_SWITCH_PIN) == LOW);
  
  // LED indicates water availability
  digitalWrite(LED_PIN, waterAvailable ? HIGH : LOW);
  
  // === SOIL MOISTURE SENSOR ===
  int rawMoisture = analogRead(SOIL_SENSOR_PIN);
  int moistureLevel = map(rawMoisture, 550, 0, 0, 100);
  moistureLevel = constrain(moistureLevel, 0, 100);
  
  // === PH SENSOR ===
  int sensorValue = analogRead(PH_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float phValue = 3.5 * voltage + phOffset;
  
  // === CRITICAL: EMERGENCY PUMP SHUTOFF ===
  // If water is not available, FORCE pump OFF regardless of mode
  if (!waterAvailable && pumpState) {
    digitalWrite(PUMP_PIN, HIGH);
    pumpState = false;
    Serial.println("EMERGENCY: Pump OFF - No Water!");
    BTSerial.println("EMERGENCY: Pump OFF - No Water!");
  }
  
  // === BUZZER LOGIC ===
  if (!waterAvailable) {
    tone(BUZZER_PIN, 750);
  } else {
    noTone(BUZZER_PIN);
  }
  
  // === DISPLAY & SERIAL UPDATES ===
  Serial.print("Water Level: ");
  Serial.println(waterAvailable ? "HIGH" : "LOW");
  Serial.print("Soil Moisture: ");
  Serial.print(moistureLevel);
  Serial.println("%");
  Serial.print("pH Value: ");
  Serial.println(phValue, 2);
  
  BTSerial.print("Water: ");
  BTSerial.print(waterAvailable ? "HIGH" : "LOW");
  BTSerial.print(" | Moisture: ");
  BTSerial.print(moistureLevel);
  BTSerial.print("% | pH: ");
  BTSerial.println(phValue, 2);
  
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(moistureLevel);
  lcd.print("% ");
  lcd.print(waterAvailable ? "W:OK " : "W:LOW");
  
  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(phValue, 1);
  lcd.setCursor(9, 1);
  lcd.print(pumpState ? "P:ON " : "P:OFF");
  
  // === BLUETOOTH CONTROL ===
  if (BTSerial.available()) {
    char command = BTSerial.read();
    
    if (command == '1') { // Manual ON
      manualControl = true;
      // Only allow manual ON if water is available
      if (waterAvailable) {
        digitalWrite(PUMP_PIN, LOW);
        pumpState = true;
        Serial.println("Pump ON (Manual)");
        BTSerial.println("Pump ON (Manual)");
      } else {
        Serial.println("Cannot turn ON - No Water!");
        BTSerial.println("Cannot turn ON - No Water!");
      }
    }
    else if (command == '0') { // Manual OFF
      manualControl = true;
      digitalWrite(PUMP_PIN, HIGH);
      pumpState = false;
      Serial.println("Pump OFF (Manual)");
      BTSerial.println("Pump OFF (Manual)");
    }
    else if (command == 'A') { // Automatic Mode
      manualControl = false;
      Serial.println("Auto Mode Enabled");
      BTSerial.println("Auto Mode Enabled");
    }
  }
  
  // === AUTOMATIC PUMP CONTROL ===
  if (!manualControl) {
    // Pump ON conditions: low moisture AND water available
    if (moistureLevel < MOISTURE_THRESHOLD && waterAvailable) {
      if (!pumpState) {
        digitalWrite(PUMP_PIN, LOW);
        pumpState = true;
        Serial.println("Pump ON (Auto)");
        BTSerial.println("Pump ON (Auto)");
      }
    } 
    // Pump OFF conditions: moisture satisfied OR no water
    else {
      if (pumpState) {
        digitalWrite(PUMP_PIN, HIGH);
        pumpState = false;
        Serial.println("Pump OFF (Auto)");
        BTSerial.println("Pump OFF (Auto)");
      }
    }
  }
  
  delay(500);
}
