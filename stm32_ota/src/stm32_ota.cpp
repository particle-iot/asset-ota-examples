/*
 * Project stm32_ota
 * Description: This is a demo project for OTA update on an STM32 microcontroller connected
 * to a P2 via UART, BOOT0 and RESET pins
 * 
 * Author: Julien Vanier
 * Date: June 2023
 */

#include <Particle.h>
#include "stm32_flash.h"


SYSTEM_THREAD(ENABLED);

PRODUCT_VERSION(1);

// handleAvailableAssets() is called when new firmware assets are available
// In this case, the asset is the STM32 binary. It will be flashed before the main program starts
void handleAvailableAssets(spark::Vector<ApplicationAsset> assets) {
  Serial.begin();
  bool flashed = false;
  for (auto& asset: assets) {
    if (asset.name == "stm32.bin") {
      // Flash the STM32 binary
      Serial.println("Flashing STM32 binary");
      flashStm32Binary(asset);
      flashed = true;
    } else {
      Serial.printlnf("Unknown asset %s", asset.name.c_str());
    }
  }

  if (!flashed) {
    Serial.println("No STM32 binary found");
  }
  System.assetsHandled(true);
}


void setup() {
  Serial.begin();
  Serial.println("Setup done");
}

void loop() {
  // nothing to do here
}