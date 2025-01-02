#include <AITINKR_AIOT_V2.h>
/*
  Example: Using the AIOTCamera Library for OV2640 ESP32 Camera

  Supported Features for OV2640 ESP32 Camera:

  1. **Image Quality and Format**
     - Brightness: Adjust image brightness (`setBrightness(int level)`).
     - Contrast: Adjust image contrast (`setContrast(int level)`).
     - Saturation: Adjust image saturation (`setSaturation(int level)`).
     - Special Effects: Apply effects like grayscale, negative, or sepia (`setSpecialEffect(int effect)`).
     - JPEG Quality: Control JPEG compression quality (`setQuality(int quality)`).
     - Pixel Format:
         - `PIXFORMAT_JPEG`: Compressed JPEG (default).
         - `PIXFORMAT_RGB565`: 16-bit RGB.
         - `PIXFORMAT_YUV422`: YUV422 format.
         - `PIXFORMAT_GRAYSCALE`: Grayscale.
         - `PIXFORMAT_RAW`: Uncompressed RAW format.

  2. **Resolution and Frame Size**
     - Configure resolutions from 96x96 to 1600x1200 using `setFrameSize(framesize_t size)`:
       - `FRAMESIZE_96X96`
       - `FRAMESIZE_QQVGA` (160x120)
       - `FRAMESIZE_QCIF` (176x144)
       - `FRAMESIZE_HQVGA` (240x176)
       - `FRAMESIZE_QVGA` (320x240)
       - `FRAMESIZE_VGA` (640x480)
       - `FRAMESIZE_SVGA` (800x600)
       - `FRAMESIZE_XGA` (1024x768)
       - `FRAMESIZE_UXGA` (1600x1200)

  3. **Exposure and White Balance**
     - Exposure Level: Adjust exposure (`setExposureLevel(int level)`).
     - Auto Exposure Control: Enable/disable automatic exposure (`setExposureControl(bool enable)`).
     - White Balance Mode: Adjust white balance (`setWhiteBalanceMode(int mode)`).
     - AWB Gain: Enable/disable auto white balance gain (`setAwbGain(bool enable)`).
     - Manual White Balance Mode: Configure specific white balance presets (e.g., daylight, cloudy).

  4. **Flip and Mirror (Rotation)**
     - Vertical Flip: Rotate the image vertically (`setFlip(bool enable)`).
     - Horizontal Mirror: Rotate the image horizontally (`setMirror(bool enable)`).

  5. **Advanced Features**
     - Lens Correction: Apply lens correction using `sensor->set_lenc()`.
     - Denoise: Enable/disable noise reduction (`sensor->set_denoise()`).
     - Gain Control: Adjust gain settings (`sensor->set_gain_ctrl()`).
     - BPC (Bad Pixel Correction): Enable/disable correction of defective pixels.
     - WPC (White Pixel Correction): Enable/disable white pixel correction.
*/



AIOTCamera camera;

void setup() {
    Serial.begin(115200);

    camera.setDebug(true); // Enable debug logs

    if (!camera.begin()) {
        Serial.println("Camera initialization failed. Check connections and configurations.");
        while (true); // Halt the program
    }

    // Minimal settings for testing
    if (!camera.setFrameSize(FRAMESIZE_QVGA)) {
        Serial.println("Failed to set frame size. Please ensure compatibility.");
    }
    if (!camera.setPixelFormat(PIXFORMAT_JPEG)) {
        Serial.println("Failed to set pixel format. Please ensure compatibility.");
    }

    Serial.println("Camera initialized and configured.");
}

void loop() {
    camera_fb_t* frame = camera.capture();
    if (frame) {
        Serial.printf("Frame captured successfully. Size: %d bytes\n", frame->len);
        camera.release(frame);
    } else {
        Serial.println("Frame capture failed. Try again.");
    }
    delay(1000); // Capture every second
}
