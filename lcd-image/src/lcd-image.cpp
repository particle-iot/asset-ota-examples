/*
 * Project lcd-image
 * Description:
 * Author:
 * Date:
 */

#include <Particle.h>
#include <Adafruit_ILI9341.h>

const auto TFT_DC = D5;
const auto TFT_CS = D4;

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

void setup() {
  tft.begin();

  // find the splash screen in the assets
  auto assets = System.assetsAvailable();
  for (auto& asset: assets) {
    if (asset.name() == "splash.img") {
      // draw the splash screen line by line
      for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
        // each pixel is a 16 bit value, with the RGB values packed 5 bits for red, 6 bits for green, 5 bits for blue
        uint16_t line[ILI9341_TFTWIDTH] = { 0 };
        char* buf = (char*) line;
        int read = 0;
        while (asset.available() && read < (int)sizeof(line)) {
          read += asset.read(&buf[read], sizeof(line) - read);
        }

        // draw the line
        tft.drawRGBBitmap(0, y, line, ILI9341_TFTWIDTH, 1);
      }
    }
  }
}

void loop() {

}