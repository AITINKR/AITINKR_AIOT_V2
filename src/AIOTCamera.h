#ifndef AIOT_CAMERA_H
#define AIOT_CAMERA_H

#include "esp_camera.h"
#include <Arduino.h>

class AIOTCamera {
public:
    // Constructor
    AIOTCamera();

    // Initialize the camera
    bool begin();

    // Enable or disable debugging
    void setDebug(bool enable);

    // Framebuffer Settings
    bool setFramebufferCount(int count);

    // Image Quality and Format
    bool setBrightness(int level);
    bool setContrast(int level);
    bool setSaturation(int level);
    bool setSpecialEffect(int effect);
    bool setQuality(int quality);
    bool setPixelFormat(pixformat_t format);

    // Resolution and Frame Size
    bool setFrameSize(framesize_t size);

    // Exposure and White Balance
    bool setWhiteBalanceMode(int mode);
    bool setAwbGain(bool enable);
    bool setExposureControl(bool enable);
    bool setExposureLevel(int level);

    // Flip and Mirror
    bool setFlip(bool enable);
    bool setMirror(bool enable);

    // Advanced Features
    bool setLensCorrection(bool enable);
    bool setDenoise(bool enable);
    bool setGainControl(bool enable);
    bool setBadPixelCorrection(bool enable);
    bool setWhitePixelCorrection(bool enable);

    // Capture and Release Frames
    camera_fb_t* capture();
    void release(camera_fb_t* fb);

private:
    // Internal Camera Configuration
    bool configureCamera();
    sensor_t* getSensor();

    // Private Variables
    int framebufferCount = 1; // Default framebuffer count
    bool debug = false;       // Debug flag
};

#endif // AIOT_CAMERA_H
