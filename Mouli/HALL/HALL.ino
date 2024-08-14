#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#define SSD1306_I2C_ADDRESS 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int hallSensorPin = 2;
volatile int pulseCount = 0;
unsigned long lastTime = 0;
unsigned long sampleTime = 1000;
float rpm = 0;

void setup() {
  Serial.begin(9600);
  pinMode(hallSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin), countPulse, FALLING);

  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= sampleTime) {
    detachInterrupt(digitalPinToInterrupt(hallSensorPin));
    rpm = (pulseCount * 60.0) / (sampleTime / 1000.0);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("RPM: ");
    display.print(rpm);
    display.display();
    pulseCount = 0;
    lastTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(hallSensorPin), countPulse, FALLING);
  }
}

void countPulse() {
  pulseCount++;
}
