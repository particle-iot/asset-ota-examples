/*
 * Project lcd-image
 * Description:
 * Author:
 * Date:
 */

#include <Particle.h>
#include <Adafruit_ILI9341.h>

SYSTEM_THREAD(ENABLED);

PRODUCT_VERSION(2);

SerialLogHandler dbg(LOG_LEVEL_NONE, {
  { "app", LOG_LEVEL_ALL }
});

#if PLATFORM_ID == PLATFORM_P2
const auto TFT_DC = S4;
const auto TFT_CS = S3;
#else
const auto TFT_DC = A4;
const auto TFT_CS = A5;
#endif

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

void setup() {
  tft.begin();

  // find the splash screen in the assets
  auto assets = System.assetsAvailable();
  for (auto& asset: assets) {
    if (asset.name() == "splash.img") {
      LOG(INFO, "Found splash.img");
      // draw the splash screen line by line
      for (int x = 0; x < ILI9341_TFTWIDTH; x++) {
        // each pixel is a 16 bit value, with the RGB values packed 5 bits for red, 6 bits for green, 5 bits for blue
        uint16_t line[ILI9341_TFTHEIGHT] = { 0 };
        char* buf = (char*) line;
        asset.read(buf, sizeof(line));

        // draw the line
        tft.drawRGBBitmap(ILI9341_TFTWIDTH - x - 1, 0, line, 1, ILI9341_TFTHEIGHT);
      }
      LOG(INFO, "Done drawing splash.img");
    }
  }
}

void loop() {

}