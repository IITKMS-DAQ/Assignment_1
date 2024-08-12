#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define HALL_PIN 2
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile unsigned int pulseCount = 0;

float wheelDiameter = 0.7; //Need to adjust
float circumference = wheelDiameter * PI;

TinyGPSPlus gps;
SoftwareSerial ss(4, 3); // RX, TX for GPS

void setup() {
    Serial.begin(115200);
    ss.begin(9600);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while(1);
    }
    display.clearDisplay();
    pinMode(HALL_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallSensorISR, FALLING);
}

void loop() {
    while (ss.available() > 0) {
        gps.encode(ss.read());
    }

    float speedRPM = calculateSpeedFromRPM();
    float speedGPS = 0.0;

    if (gps.speed.isValid())        speedGPS = gps.speed.kmph();

    float finalSpeed = combineSpeeds(speedRPM, speedGPS);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Speed: ");
    display.print(finalSpeed);
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

float calculateSpeedFromRPM() {
    if (pulseInterval == 0) return 0.0;
    float rotationsPerSecond = 1000000.0 / pulseInterval;
    float speedMps = rotationsPerSecond * circumference;
    float speedKmph = speedMps * 3.6;
    return speedKmph;
}

float combineSpeeds(float speedRPM, float speedGPS) {
    if (speedGPS > 0.0) {
        float slipThreshold = 5.0; //Adjust just as needed according to data
        if (abs(speedRPM - speedGPS) > slipThreshold)     return speedGPS;
        else   return (speedRPM * 0.7 + speedGPS * 0.3);
    }
    return speedRPM;
}