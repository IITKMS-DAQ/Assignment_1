#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define HALL_PIN 2 // Hall effect sensor connected to digital pin 2
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile unsigned int pulseCount = 0;

float wheelDiameter = 0.7; //Need to adjust
float circumference = wheelDiameter * PI;

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  display.clearDisplay();

  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallSensorISR, FALLING);
}

void loop() {
  float speed = calculateSpeed();
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Speed: ");
  display.print(speed);
  display.print(" km/h");

  display.display();

  delay(100);
}

void hallSensorISR() {
  unsigned long currentTime = micros();
  pulseInterval = currentTime - lastPulseTime;
  lastPulseTime = currentTime;
  pulseCount++;
}

float calculateSpeed() {
  if (pulseInterval == 0) return 0.0;
  float rotationsPerSecond = 1000000.0 / pulseInterval;
  float speedMps = rotationsPerSecond * circumference;
  float speedKmph = speedMps * 3.6;
  return speedKmph;
}
