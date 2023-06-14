/*
 * Project lcd-image
 * Description:
 * Author:
 * Date:
 */

#include <Particle.h>
#include <Adafruit_ILI9341.h>

#define TFT_DC   D5
#define TFT_CS   D4
#define STMPE_CS D3
#define SD_CS    D2

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

uint16_t bitmap[ILI9341_TFTWIDTH] = { 0 };

void setup() {
  tft.begin();

  // find the logo in the assets
  auto assets = System.assetsAvailable();

  for (auto& asset: assets) {
    if (asset.name() == "image.dat") {
      // draw the logo line by line
      for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
        // each pixel is 2 bytes, with the RGB values packed 5 bits for red, 6 bits for green, 5 bits for blue
        char buf[ILI9341_TFTWIDTH * sizeof(uint16_t)] = { 0 };
        int read = 0;
        while (asset.available() && read < (int)sizeof(buf)) {
          read += asset.read(&buf[read], sizeof(buf) - read);
        }

        tft.drawRGBBitmap(0, y, (uint16_t*) &buf, ILI9341_TFTWIDTH, 1);
      }
    }
  }
}

void loop() {

}