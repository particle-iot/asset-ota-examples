# Asset OTA examples

Particle Asset OTA (available in Device OS 5.5.0 and later), makes it possible to include bundled assets in an OTA software update that can be delivered to other processors and components in your product. Bundled assets can be up to 1 MB to 1.5 MB in size, after compression, depending on platform and do not use additional data operations.

With this feature, your Particle device can not only update itself, but also update the components connected to it!

Example applications using the Asset OTA functionality include:

## Coprocessor update

The [stm32_ota](/stm32_ota) project shows how to bundle a firmware binary for a coprocessor (in this case an STM32 microcontroller) and how to update the coprocessor when a new binary has been received.

The application registers a callback using `System.onAssetOta(handleAssets)`. `handleAssets` will find the firmware binary for the coprocessor, flash it, then mark the assets as handled using `System.assetsHandled(true)` so that `handleAssets` is not called on next boot.

To integrate this example into your project, use the [STM32_Flash library](https://github.com/particle-iot/STM32_Flash).

## LCD image resource

The [lcd_image](/lcd_image) project shows how to send resources like graphics, fonts, sound samples as assets. The application will not start until all the assets have been delivered to the device so it's safe to assume that the assets are available.

In this case, the application searches the lists of available assets using `System.assetsAvailable()` and read a splash screen image directly from asset storage into the memory of an external LCD screen.

## Learn more

For an introduction to Asset OTA including its use cases, creating an asset bundle, and example projects, see [Asset OTA](https://docs.particle.io/reference/asset-ota/asset-ota/).

The Device OS firmware API reference includes information about:

- [`System.onAssetOta()`](https://docs.particle.io/reference/device-os/api/system-calls/onassetota-system/) 
- [`System.assetsHandled()`](https://docs.particle.io/reference/device-os/api/system-calls/assetshandled-system/)

