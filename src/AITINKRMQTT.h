#ifndef AITINKRMQTT_H
#define AITINKRMQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_camera.h"  // Include for camera frame buffer support

class AITINKRMQTT : public PubSubClient {
public:
    AITINKRMQTT(const char* ssid, const char* password, const char* userTopic);

    void begin();
    void loop();

    // Status checking
    bool isWiFiConnected();
    bool isMQTTConnected();

    // Topic publishing and subscribing
    bool publishToSubtopic(const char* subtopic, const char* message); // Original simplified version
    bool publishToSubtopic(const char* subtopic, const uint8_t* payload, unsigned int plength); // Original binary payload version
    bool publishToSubtopic(const char* subtopic, const char* message, uint8_t qos); // Extended with QoS
    bool publishToSubtopic(const char* subtopic, const uint8_t* payload, unsigned int plength, uint8_t qos); // Extended with QoS

    void publishToSubtopicVoid(const char* subtopic, const char* message);
    void publishImageFromCameraBuffer(camera_fb_t* fb); // Original simplified version
    bool publishImageFromCameraBuffer(camera_fb_t* fb, uint8_t qos); // Extended with QoS

    bool subscribeToSubtopic(const char* subtopic);
    bool subscribeToSubtopic(const char* subtopic, uint8_t qos);
    bool unsubscribeToSubtopic(const char* subtopic);

    // Predefined topics
    void sendStatus(const char* message = "{\"status\":\"connected\"}");
    bool subscribeToGetData();
    bool unsubscribeToGetData();
    bool subscribeToGetData(uint8_t qos);

    // Buffer size configuration
    void setPublishBufferSize(uint16_t size);
    uint16_t getPublishBufferSize() const;

    // Periodic status updates
    void setStatusUpdateInterval(unsigned long intervalMillis);

    // Set the callback for handling incoming messages
    void setCallback(MQTT_CALLBACK_SIGNATURE);

    // Extra features: Debugging and Auto-Reconnect Control
    void setDebugEnabled(bool enabled);
    void setAutoReconnect(bool enabled);

    bool aiotmqttDebugEnabled = true;    // Debug enabled by default
    bool aiotmqttAutoReconnect = true;   // Auto-reconnect enabled by default

private:
    bool connectToWiFi();
    bool connectToMQTT();
    void ensureMQTTConnected();
    void sendStatusUpdate();

    const char* _ssid;
    const char* _password;
    const char* _userTopic;

    WiFiClient _wifiClient;

    static constexpr const char* MQTT_SERVER = "mqtt.schoolforai.com";
    static constexpr int MQTT_PORT = 1883;

    uint16_t _publishBufferSize = 1024; // Default size
    unsigned long _statusIntervalMillis = 0;
    unsigned long _lastStatusUpdate = 0;
};

#endif  // AITINKRMQTT_H
