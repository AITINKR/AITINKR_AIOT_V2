#include <AITINKR_AIOT_V2.h>

// Wi-Fi and MQTT credentials
const char* ssid = "your_SSID";       // Replace with your Wi-Fi SSID
const char* password = "your_PASS";   // Replace with your Wi-Fi password
const char* userTopic = "deviceID";   // The base topic for your device's MQTT communication

// Create an instance of AITINKRMQTT
AITINKRMQTT mqttClient(ssid, password, userTopic);

// MQTT Callback Function
// This function is called when a message arrives on a subscribed topic
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); // Print each character of the payload
  }
  Serial.println();
}

// Uncomment and configure ESP32 camera to use this function
/*
void publishCameraImage() {
  // Capture an image from the ESP32 camera
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture image.");
    return;
  }

  // Publish the image buffer to the "camera" topic with QoS 1
  if (mqttClient.publishImageFromCameraBuffer(fb, 1)) {
    Serial.println("Camera image published successfully.");
  } else {
    Serial.println("Failed to publish camera image.");
  }

  esp_camera_fb_return(fb); // Release the frame buffer
}
*/

void setup() {
  Serial.begin(115200);

  // Initialize the MQTT client (connect to Wi-Fi and MQTT broker)
  mqttClient.begin();

  // Set the callback function to handle incoming messages
  mqttClient.setCallback(mqttCallback);

  // Set the buffer size for publishing large messages
  mqttClient.setPublishBufferSize(512);

  // Enable debugging for detailed logs
  mqttClient.setDebugEnabled(false);

  // Enable auto-reconnect for Wi-Fi and MQTT
  mqttClient.setAutoReconnect(true);

  // Subscribe to a custom subtopic (e.g., "todevice")
  if (mqttClient.subscribeToSubtopic("todevice", 1)) {
    Serial.println("Subscribed to /todevice with QoS 1.");
  } else {
    Serial.println("Failed to subscribe to /todevice.");
  }

  // Publish an initial retained message to the "status" topic
  if (mqttClient.publishToSubtopic("status", "{\"message\":\"Device Online\"}", 1)) {
    Serial.println("Retained message published to 'status'.");
  } else {
    Serial.println("Failed to publish retained message.");
  }

  // Uncomment this line to publish a test camera image
  // publishCameraImage();
}

void loop() {
  // Handle incoming messages and maintain the connection
  mqttClient.loop();

  // Periodically publish a status update to a custom topic
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate >= 10000) { // Every 10 seconds
    if (mqttClient.publishToSubtopic("status", "{\"status\":\"Running\"}")) {
      Serial.println("Periodic status update sent to 'status'.");
    } else {
      Serial.println("Failed to send periodic status update.");
    }
    lastStatusUpdate = millis();
  }
}
