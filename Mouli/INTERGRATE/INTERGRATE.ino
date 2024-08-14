#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#define SSD1306_I2C_ADDRESS 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SoftwareSerial gpsSerial(4, 3);
TinyGPSPlus gps;

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int hallSensorPin = 2;
volatile int pulseCount = 0;
unsigned long lastTime = 0;
unsigned long sampleTime = 1000;
float rpm = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  pinMode(hallSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin), countPulse, FALLING);

  if (!bno.begin()) {
    while (1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);

  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("Lat: ");
      display.print(gps.location.lat(), 6);
      display.setCursor(0, 10);
      display.print("Lon: ");
      display.print(gps.location.lng(), 6);
      display.setCursor(0, 20);
      display.print("Alt: ");
      display.print(gps.altitude.meters());
      display.setCursor(0, 30);
      display.print("Spd: ");
      display.print(gps.speed.kmph());
      display.setCursor(0, 40);

      unsigned long currentTime = millis();

      if (currentTime - lastTime >= sampleTime) {
        detachInterrupt(digitalPinToInterrupt(hallSensorPin));
        rpm = (pulseCount * 60.0) / (sampleTime / 1000.0);
        display.print("RPM: ");
        display.print(rpm);
        pulseCount = 0;
        lastTime = currentTime;
        attachInterrupt(digitalPinToInterrupt(hallSensorPin), countPulse, FALLING);
      }

      imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
      float yaw = euler.x();
      float pitch = euler.y();
      float roll = euler.z();

      display.setCursor(0, 50);
      display.print("Yaw: ");
      display.print(yaw);
      display.setCursor(0, 60);
      display.print("Pitch: ");
      display.print(pitch);
      display.setCursor(0, 70);
      display.print("Roll: ");
      display.print(roll);
      display.display();
    }
  }
}

void countPulse() {
  pulseCount++;
}
