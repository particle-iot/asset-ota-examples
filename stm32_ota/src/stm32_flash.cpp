/*
 * Library to flash an asset to an STM32 microcontroller using UART, BOOT0 and RESET pins
 */

#include "stm32_flash.h"

// Hardware configuration
const auto BOOT0_PIN D7
const auto RESET_PIN D6

// STM32 communication parameters
const auto BAUD_RATE = 115200;
const auto ENTER_BOOTLOADER_TIMEOUT = 200;
const auto COMMAND_TIMEOUT = 1000;
const auto ERASE_FLASH_TIMEOUT = 5000;
const auto WRITE_BLOCK_TIMEOUT = 1000;

// STM32 bootloader commands
const uint8_t WRITE_MEMORY = 0x31;
const uint8_t ERASE_FLASH = 0x43;
const uint8_t ENTER_BOOTLOADER = 0x7F;
const uint8_t ACK = 0x79;

const uint8_t ALL_BLOCKS = 0xFF;

// STM32 memory map
const uint32_t FLASH_START = 0x08000000;

void setupStm32();
int enterBootloader();
int eraseFlash();
int flashMemory(ApplicationAsset& asset);
int writeBlock(uint32_t address, uint8_t* data, uint16_t size);
int sendCommand(uint8_t command, uint16_t timeout);
int waitForAck(uint8_t command, int timeout);


int flashStm32Binary(ApplicationAsset& asset) {
    setupStm32();
    CHECK(enterBootloader());
    CHECK(eraseFlash());
    CHECK(flashMemory(asset));
    return SYSTEM_ERROR_NONE;
}

void setupStm32() {
  // Set the GPIO pins as output
  pinMode(BOOT0_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  // Initialize the UART
  Serial1.begin(BAUD_RATE);
}

int enterBootloader() {
  LOG(INFO, "Resetting STM32 into bootloader mode");

  // Set BOOT0 pin to enter the bootloader mode
  digitalWrite(BOOT0_PIN, HIGH);

  // Reset the STM32
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);


  // Send the bootloader start command
  Serial1.write(ENTER_BOOTLOADER);

  // Wait to get acknowledgement
  return waitForAck(ENTER_BOOTLOADER, ENTER_BOOTLOADER_TIMEOUT);
}

int eraseFlash() {
  LOG(INFO, "Erasing STM32 flash");
  CHECK(sendCommand(ERASE_FLASH));
  CHECK(sendCommand(ALL_BLOCKS, ERASE_FLASH_TIMEOUT));
}

int flashMemory(ApplicationAsset& asset) {
  LOG(INFO, "Flashing STM32 binary");
  auto address = FLASH_START;

  while (asset.available()) {
    uint8_t buf[256];
    auto read = asset.read(buf, sizeof(buf));

    CHECK(writeBlock(address, buf, read));
    address += read;
  }
  LOG_WRITE(INFO, "\n");
  LOG(INFO, "Flashing complete");

  return SYSTEM_ERROR_NONE;
}

int writeBlock(uint32_t address, uint8_t* data, uint16_t size) {
  LOG_WRITE(INFO, ".");

  // Send the write command
  CHECK(sendCommand(WRITE_MEMORY));

  // write the address and checksum
  uint8_t buf[5];
  buf[0] = address >> 24;
  buf[1] = (address >> 16) & 0xFF;
  buf[2] = (address >> 8) & 0xFF;
  buf[3] = address & 0xFF;
  buf[4] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
  Serial1.write(buf, sizeof(buf));

  CHECK(waitForAck(WRITE_MEMORY, COMMAND_TIMEOUT));

  // send the number of bytes, the data and the checksum
  uint16_t alignedSize = (size + 3) & ~0x03;
  uint8_t sizeTx = (alignedSize - 1) & 0xFF;
  Serial1.write(&sizeTx, sizeof(sizeTx));
  Serial1.write(data, size);
  for (auto i = size; i < alignedSize; i++) {
    Serial1.write(0xFF);
  }

  uint8_t checksum = sizeTx;
  for (auto i = 0; i < size; i++) {
    checksum ^= data[i];
  }
  for (auto i = size; i < alignedSize; i++) {
    checksum ^= 0xFF;
  }
  Serial1.write(checksum);

  CHECK(waitForAck(WRITE_MEMORY, WRITE_BLOCK_TIMEOUT));

  return SYSTEM_ERROR_NONE;
}

int sendCommand(uint8_t command, uint16_t timeout = COMMAND_TIMEOUT) {
  // each command must be written twice: once as is and once with the complement
  uint8_t buf[] = { command, command ^ 0xFF };
  Serial1.write(buf, sizeof(buf));

  // wait for response
  return waitForAck(command, timeout);
}

int waitForAck(uint8_t command, int timeout) {
  waitFor(Serial1.available, timeout);
  if (!Serial1.available()) {
    LOG(ERROR, "Timeout waiting for STM32 to acknowledge command 0x%02x", command);
    return SYSTEM_ERROR_TIMEOUT;
  }

  auto resp = Serial1.read();
  if (resp != ACK) {
    LOG(ERROR, "Unexpected response from STM32 after command 0x%02x: 0x%02x", command, resp);
    return SYSTEM_ERROR_INVALID_STATE;
  }

  return SYSTEM_ERROR_NONE;
}