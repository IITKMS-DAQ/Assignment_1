#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <utility/imumaths.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void setup() {
  Serial.begin(115200);
  if(!bno.begin()) {
    Serial.print("No BNO055 detected. Check your wiring or I2C address.");
    while(1);
  }

  bno.setExtCrystalUse(true);

  // Set the sensor's operating mode to NDOF (9 degrees of freedom)
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

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
  // Get a vector for Euler angles (roll, pitch, yaw)
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  // Get a vector for linear acceleration (no gravity)
  imu::Vector<3> linacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // Get a vector for gyroscope (angular velocity)
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Euler Angles:");
  display.setCursor(0, 10);
  display.print("X: "); display.print(euler.x());
  display.setCursor(0, 20);
  display.print("Y: "); display.print(euler.y());
  display.setCursor(0, 30);
  display.print("Z: "); display.print(euler.z());

  display.setCursor(0, 40);
  display.print("Lin Acc:");
  display.setCursor(0, 50);
  display.print("X: "); display.print(linacc.x());
  display.setCursor(0, 60);
  display.print("Y: "); display.print(linacc.y());

  display.display();

  delay(100);
}
