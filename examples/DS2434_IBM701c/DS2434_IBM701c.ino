/* Example-Code that emulates a DS2434 with Battery Management & EEPROM

Tested with
- IBM 701c - due to special non-standard onewire-usage

Compile-Sizes with platformIO
pio ci --lib='.' -b digispark-tiny .\examples\DS2434_IBM701c\DS2434_IBM701c.ino

    OneWireHub
    RAM:   [========  ]  76.6% (used 392 bytes from 512 bytes)
    Flash: [=======   ]  66.4% (used 3994 bytes from 6012 bytes)

    OneWireHub-Tiny
    RAM:   [======    ]  57.4% (used 294 bytes from 512 bytes)
    Flash: [====      ]  40.8% (used 2452 bytes from 6012 bytes)

Programming with Platform-IO:
platformio.exe ci -b digispark-tiny .\examples\DS2434_IBM701c\DS2434_IBM701c.ino --build-dir ./build --keep-build-dir
# creates a full pio-project
cd build
pio run --target upload

*/

#include "OneWireHub.h"
#include "DS2434.h"

constexpr uint8_t pin_onewire   { 2 };

auto hub = OneWireHub(pin_onewire);
auto ds2434 = DS2434();

void setup()
{
    // Setup OneWire
    hub.attach(ds2434);

    // add default-data
    constexpr uint8_t mem1[24] = {0x14, 0x10, 0x90, 0xd0, 0x03, 0x32, 0x4b, 0x3c,
                                  0xff, 0x04, 0x64, 0x04, 0x9e, 0x9a, 0x3a, 0xf0,
                                  0x20, 0x20, 0x04, 0xee, 0x63, 0xB8, 0x3E, 0x63 };  // last 4 Byte seem to be Serial
    ds2434.writeMemory(reinterpret_cast<const uint8_t *>(mem1),sizeof(mem1),0x00);

    constexpr  uint8_t mem2[8] = {0x33, 0x2e, 0x33, 0x2e, 0x9e, 0x10, 0x3f, 0x50 };
    ds2434.writeMemory(reinterpret_cast<const uint8_t *>(mem2),sizeof(mem2),0x20);

    ds2434.lockNV1();
    ds2434.setID(0xCABDu);
    ds2434.setBatteryCounter(0x0123u);
    ds2434.setTemperature(22u);
}

void loop()
{
    static uint8_t temp = 20u;

    // following function must be called periodically
    hub.poll();

    // demo: every read increases the Temp
    if (ds2434.getTemperatureRequest())
    {
        ds2434.setTemperature(temp++);
        if (temp > 25u) temp = 20u;
    }

}
