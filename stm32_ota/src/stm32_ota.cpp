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

SerialLogHandler dbg(LOG_LEVEL_ALL);


// Hardware configuration
const auto BOOT0_PIN = MOSI;
const auto BOOT1_PIN = MISO;
const auto RESET_PIN = SCK;

void initStm32();

// handleAvailableAssets() is called when new firmware assets are available
// In this case, the asset is the STM32 binary. It will be flashed before the main program starts
void handleAvailableAssets(spark::Vector<ApplicationAsset> assets) {
  initStm32();
  Serial.begin();
  delay(1000);
  bool flashed = false;
  for (auto& asset: assets) {
    if (asset.name() == "stm32.bin") {
      // Flash the STM32 binary
      Serial.println("Flashing STM32 binary");
      flashStm32Binary(asset, BOOT0_PIN, RESET_PIN);
      flashed = true;
    } else {
      Serial.printlnf("Unknown asset %s", asset.name().c_str());
    }
  }

  if (!flashed) {
    Serial.println("No STM32 binary found");
  }
  System.assetsHandled(true);
}

void initStm32() {
  pinMode(BOOT0_PIN, OUTPUT);
  pinMode(BOOT1_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  // Set BOOT0 and BOOT1 to 0 and RESET to 1
  digitalWrite(BOOT0_PIN, LOW);
  digitalWrite(BOOT1_PIN, LOW);
  digitalWrite(RESET_PIN, HIGH);
}

void setup() {
  initStm32();

  Serial.begin();
  delay(1000);
  Serial.println("Setup done");

  // redo assets handling on next boot for demo purposes
  System.assetsHandled(false);
}

void loop() {
  // nothing to do here
}