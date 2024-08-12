#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);

void setup() {
  Serial.begin(115200);
  ss.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }

  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
  display.clearDisplay();

  if (gps.speed.isValid()){
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Speed: ");
    display.print(gps.speed.kmph());
    display.print(" km/h");
  } 
  else{
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("No Speed Data");
  }

  display.display();
  delay(100);
}
