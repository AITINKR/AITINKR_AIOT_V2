#include "AITINKRMQTT.h"

AITINKRMQTT::AITINKRMQTT(const char* ssid, const char* password, const char* userTopic)
    : _ssid(ssid), _password(password), _userTopic(userTopic), PubSubClient(_wifiClient) {}

void AITINKRMQTT::begin() {
    connectToWiFi();
    setBufferSize(1024);
    setServer(MQTT_SERVER, MQTT_PORT);
    setBufferSize(_publishBufferSize);
    if (aiotmqttDebugEnabled) {
        Serial.println("MQTT client initialized.");
    }
}

void AITINKRMQTT::loop() {
    if (!isWiFiConnected()) {
        if (aiotmqttAutoReconnect) {
            connectToWiFi();
        } else {
            return;
        }
    }
    ensureMQTTConnected();
    sendStatusUpdate();
    PubSubClient::loop();
}

bool AITINKRMQTT::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool AITINKRMQTT::isMQTTConnected() {
    return connected();
}

// Original publishing methods
bool AITINKRMQTT::publishToSubtopic(const char* subtopic, const char* message) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    if (publish(topic.c_str(), message)) {
        if (aiotmqttDebugEnabled) {
            Serial.println("Published to: " + topic + ", Message: " + String(message));
        }
        return true;
    } else {
        if (aiotmqttDebugEnabled) {
            Serial.println("Failed to publish to: " + topic);
        }
        return false;
    }
}

bool AITINKRMQTT::publishToSubtopic(const char* subtopic, const uint8_t* payload, unsigned int plength) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    if (publish(topic.c_str(), payload, plength)) {
        if (aiotmqttDebugEnabled) {
            Serial.println("Binary payload published to: " + topic);
        }
        return true;
    } else {
        if (aiotmqttDebugEnabled) {
            Serial.println("Failed to publish binary payload to: " + topic);
        }
        return false;
    }
}

// Extended publishing methods with QoS
bool AITINKRMQTT::publishToSubtopic(const char* subtopic, const char* message, uint8_t qos) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    if (publish(topic.c_str(), message)) { // QoS is simulated as `PubSubClient` does not support QoS handling internally.
        if (aiotmqttDebugEnabled) {
            Serial.println("Published with QoS to: " + topic + ", Message: " + String(message) + ", QoS: " + String(qos));
        }
        return true;
    } else {
        if (aiotmqttDebugEnabled) {
            Serial.println("Failed to publish with QoS to: " + topic);
        }
        return false;
    }
}

bool AITINKRMQTT::publishToSubtopic(const char* subtopic, const uint8_t* payload, unsigned int plength, uint8_t qos) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    if (publish(topic.c_str(), payload, plength)) { // QoS is simulated
        if (aiotmqttDebugEnabled) {
            Serial.println("Binary payload published with QoS to: " + topic + ", QoS: " + String(qos));
        }
        return true;
    } else {
        if (aiotmqttDebugEnabled) {
            Serial.println("Failed to publish binary payload with QoS to: " + topic);
        }
        return false;
    }
}

// Simplified publishing method for camera buffer
void AITINKRMQTT::publishImageFromCameraBuffer(camera_fb_t* fb) {
    if (fb != nullptr) {
        setBufferSize(fb->len + 200);
        publishToSubtopic("camera", fb->buf, fb->len);
    } else if (aiotmqttDebugEnabled) {
        Serial.println("Frame buffer is null. Cannot publish.");
    }
}

// Extended publishing method for camera buffer with QoS
bool AITINKRMQTT::publishImageFromCameraBuffer(camera_fb_t* fb, uint8_t qos) {
    if (fb == nullptr) {
        if (aiotmqttDebugEnabled) {
            Serial.println("Frame buffer is null. Cannot publish.");
        }
        return false;
    }
    setBufferSize(fb->len + 200);
    return publishToSubtopic("camera", fb->buf, fb->len, qos);
}

void AITINKRMQTT::publishToSubtopicVoid(const char* subtopic, const char* message) {
    publishToSubtopic(subtopic, message);
}

bool AITINKRMQTT::subscribeToSubtopic(const char* subtopic) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    return subscribe(topic.c_str());
}

bool AITINKRMQTT::subscribeToSubtopic(const char* subtopic, uint8_t qos) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    return subscribe(topic.c_str(), qos);
}

bool AITINKRMQTT::unsubscribeToSubtopic(const char* subtopic) {
    ensureMQTTConnected();
    String topic = String(_userTopic) + "/" + String(subtopic);
    return unsubscribe(topic.c_str());
}

void AITINKRMQTT::sendStatus(const char* message) {
    publishToSubtopic("status", message);
}

bool AITINKRMQTT::subscribeToGetData() {
    return subscribeToSubtopic("toDevice");
}

bool AITINKRMQTT::unsubscribeToGetData() {
    return unsubscribeToSubtopic("toDevice");
}

bool AITINKRMQTT::subscribeToGetData(uint8_t qos) {
    return subscribeToSubtopic("toDevice", qos);
}

void AITINKRMQTT::setPublishBufferSize(uint16_t size) {
    _publishBufferSize = size;
    setBufferSize(size);
    if (aiotmqttDebugEnabled) {
        Serial.println("Buffer size set to: " + String(size));
    }
}

uint16_t AITINKRMQTT::getPublishBufferSize() const {
    return _publishBufferSize;
}

void AITINKRMQTT::setStatusUpdateInterval(unsigned long intervalMillis) {
    _statusIntervalMillis = intervalMillis;
}

void AITINKRMQTT::sendStatusUpdate() {
    if (_statusIntervalMillis > 0 && millis() - _lastStatusUpdate >= _statusIntervalMillis && isMQTTConnected()) {
        sendStatus();
        _lastStatusUpdate = millis();
    }
}

bool AITINKRMQTT::connectToWiFi() {
    if (aiotmqttDebugEnabled) {
        Serial.print("Connecting to Wi-Fi...");
    }
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        if (aiotmqttDebugEnabled) {
            Serial.print(".");
        }
    }
    if (aiotmqttDebugEnabled) {
        Serial.println("\nWi-Fi connected.");
    }
    return true;
}

bool AITINKRMQTT::connectToMQTT() {
    while (!connected()) {
        if (aiotmqttDebugEnabled) {
            Serial.print("Connecting to MQTT broker...");
        }
        String clientId = "Client-" + String(random(0xffff), HEX);
        if (connect(clientId.c_str())) {
            if (aiotmqttDebugEnabled) {
                Serial.println("MQTT broker connected.");
            }
            return true;
        } else {
            if (aiotmqttDebugEnabled) {
                Serial.print("Failed to connect. Error: ");
                Serial.println(state());
            }
            delay(5000);
        }
    }
    return false;
}

void AITINKRMQTT::ensureMQTTConnected() {
    if (!isMQTTConnected()) {
        if (aiotmqttDebugEnabled) {
            Serial.println("MQTT disconnected. Reconnecting...");
        }
        connectToMQTT();
    }
}

void AITINKRMQTT::setCallback(MQTT_CALLBACK_SIGNATURE) {
    PubSubClient::setCallback(callback);
    if (aiotmqttDebugEnabled) {
        Serial.println("Callback set successfully.");
    }
}

void AITINKRMQTT::setDebugEnabled(bool enabled) {
    aiotmqttDebugEnabled = enabled;
    if (aiotmqttDebugEnabled) {
        Serial.println("Debugging enabled.");
    } else {
        Serial.println("Debugging disabled.");
    }
}

void AITINKRMQTT::setAutoReconnect(bool enabled) {
    aiotmqttAutoReconnect = enabled;
    if (aiotmqttDebugEnabled) {
        Serial.print("Auto-reconnect ");
        Serial.println(aiotmqttAutoReconnect ? "enabled." : "disabled.");
    }
}