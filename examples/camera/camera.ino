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

    if (camera.begin()) {
        Serial.println("AIOTCamera ready!");

        // Configure image settings
        camera.setBrightness(2);                // Increase brightness
        camera.setContrast(1);                  // Adjust contrast
        camera.setSaturation(2);                // Increase saturation
        camera.setSpecialEffect(1);             // Apply negative effect
        camera.setFlip(true);                   // Enable vertical flip
        camera.setMirror(true);                 // Enable horizontal mirror
        camera.setQuality(10);                  // Set high-quality JPEG
        camera.setFrameSize(FRAMESIZE_VGA);     // Set resolution to VGA
        camera.setPixelFormat(PIXFORMAT_GRAYSCALE); // Set grayscale format

        // Configure advanced settings
        camera.setWhiteBalanceMode(1);          // Set white balance to daylight
        camera.setAwbGain(true);                // Enable AWB gain
        camera.setExposureControl(true);        // Enable auto exposure
        camera.setExposureLevel(2);             // Adjust exposure level
    } else {
        Serial.println("Failed to initialize AIOTCamera.");
    }
}

void loop() {
    camera_fb_t* fb = camera.capture();
    if (fb) {
        Serial.printf("Captured %d bytes\n", fb->len);
        camera.release(fb);
    } else {
        Serial.println("Capture failed.");
    }
    delay(1000);
}
