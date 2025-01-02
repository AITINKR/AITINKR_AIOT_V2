
#include "AIOTCamera.h"

// Fixed GPIO pin definitions for OV2640

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 10
#define SIOD_GPIO_NUM 21
#define SIOC_GPIO_NUM 14

#define Y2_GPIO_NUM 16
#define Y3_GPIO_NUM 7
#define Y4_GPIO_NUM 6
#define Y5_GPIO_NUM 15
#define Y6_GPIO_NUM 17
#define Y7_GPIO_NUM 8
#define Y8_GPIO_NUM 9
#define Y9_GPIO_NUM 11

#define VSYNC_GPIO_NUM 13
#define HREF_GPIO_NUM 12
#define PCLK_GPIO_NUM 18

// Constructor
AIOTCamera::AIOTCamera() {}

// Enable or disable debugging
void AIOTCamera::setDebug(bool enable) {
    debug = enable;
    if (debug) {
        Serial.println("Debugging enabled.");
    }
}

// Begin: Initialize the camera
bool AIOTCamera::begin() {
    if (debug) {
        Serial.println("Initializing camera...");
    }

    if (!configureCamera()) {
        Serial.println("[Error] Camera initialization failed!");
        return false;
    }

    Serial.println("[Success] Camera initialized successfully.");
    return true;
}

// Configure Camera Settings
bool AIOTCamera::configureCamera() {
    camera_config_t config = {};
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    // Assign fixed pins
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;

    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;

    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;

    config.xclk_freq_hz = 20000000; // 20MHz clock
    config.pixel_format = PIXFORMAT_JPEG; // Default to JPEG format

    // Dynamic resolution and frame buffer settings
    if (psramFound()) {
        if (debug) {
            Serial.println("PSRAM detected. Configuring for high resolution.");
        }
        config.frame_size = FRAMESIZE_UXGA; // 1600x1200
        config.jpeg_quality = 10;          // High quality
    } else {
        if (debug) {
            Serial.println("PSRAM not detected. Configuring for medium resolution.");
        }
        config.frame_size = FRAMESIZE_SVGA; // 800x600
        config.jpeg_quality = 15;          // Medium quality
    }

    config.fb_count = framebufferCount; // Number of frame buffers

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        if (debug) {
            Serial.printf("[Error] Camera initialization failed: %s\n", esp_err_to_name(err));
        }
        return false;
    }

    if (debug) {
        Serial.println("[Info] Camera configured successfully.");
    }
    return true;
}

// Framebuffer Settings
bool AIOTCamera::setFramebufferCount(int count) {
    if (count > 0) {
        framebufferCount = count;
        if (debug) {
            Serial.printf("Framebuffer count set to: %d\n", framebufferCount);
        }
        return true;
    } else {
        if (debug) {
            Serial.println("Invalid framebuffer count. Must be greater than 0.");
        }
        return false;
    }
}

// Get Sensor
sensor_t* AIOTCamera::getSensor() {
    return esp_camera_sensor_get();
}

// Image Quality and Format
bool AIOTCamera::setBrightness(int level) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_brightness(sensor, level);
        return true;
    }
    return false;
}

bool AIOTCamera::setContrast(int level) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_contrast(sensor, level);
        return true;
    }
    return false;
}

bool AIOTCamera::setSaturation(int level) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_saturation(sensor, level);
        return true;
    }
    return false;
}

bool AIOTCamera::setSpecialEffect(int effect) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_special_effect(sensor, effect);
        return true;
    }
    return false;
}

bool AIOTCamera::setQuality(int quality) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_quality(sensor, quality);
        return true;
    }
    return false;
}

bool AIOTCamera::setPixelFormat(pixformat_t format) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_pixformat(sensor, format);
        return true;
    }
    return false;
}

// Resolution and Frame Size
bool AIOTCamera::setFrameSize(framesize_t size) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_framesize(sensor, size);
        return true;
    }
    return false;
}

// Exposure and White Balance
bool AIOTCamera::setWhiteBalanceMode(int mode) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_wb_mode(sensor, mode);
        return true;
    }
    return false;
}

bool AIOTCamera::setAwbGain(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_awb_gain(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setExposureControl(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_aec2(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setExposureLevel(int level) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_ae_level(sensor, level);
        return true;
    }
    return false;
}

// Flip and Mirror
bool AIOTCamera::setFlip(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_vflip(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setMirror(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_hmirror(sensor, enable);
        return true;
    }
    return false;
}

// Advanced Features
bool AIOTCamera::setLensCorrection(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_lenc(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setDenoise(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_denoise(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setGainControl(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_gain_ctrl(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setBadPixelCorrection(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_bpc(sensor, enable);
        return true;
    }
    return false;
}

bool AIOTCamera::setWhitePixelCorrection(bool enable) {
    sensor_t* sensor = getSensor();
    if (sensor) {
        sensor->set_wpc(sensor, enable);
        return true;
    }
    return false;
}

// Capture and Release Frames
camera_fb_t* AIOTCamera::capture() {
    camera_fb_t* frame = esp_camera_fb_get();
    if (!frame) {
        if (debug) {
            Serial.println("[Error] Frame capture failed.");
        }
    } else {
        if (debug) {
            Serial.println("[Success] Frame captured.");
        }
    }
    return frame;
}

void AIOTCamera::release(camera_fb_t* fb) {
    if (fb) {
        esp_camera_fb_return(fb);
        if (debug) {
            Serial.println("[Info] Frame buffer released.");
        }
    }
}
