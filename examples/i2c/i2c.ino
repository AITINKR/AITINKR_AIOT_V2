#include <AITINKR_AIOT_V2.h>

void setup() {
  Serial.begin(115200);
  delay(2500);
  // Initialize I2C with default pins
  initI2C();

  // Scan for I2C devices
  if (!scanI2C()) {
    Serial.println("No devices detected. Check connections!");
  }
}

void loop() {
  // Add your I2C-related tasks or interactions here
  delay(5000);
}
