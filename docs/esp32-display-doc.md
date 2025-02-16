# ESP32 1.28" Circular TFT Display Development Board
*Version: V1.01 (2023/07/12)*

## Revision History
| Date | Version | Description |
|------|---------|-------------|
| 2023-08-31 | V1.0 | Initial release |
| 2023-11-07 | V1.01 | Added additional circuit descriptions for user modifications |

## 1. Product Introduction
The development board features a 1.28-inch circular TFT screen with a GC9A01 driver chip, offering 240x240 resolution and 65K color display for enhanced clarity. The screen connects to the onboard ESP32 via SPI interface, with provided driver examples for easy implementation.

The board includes a USB port for one-click program downloads and serial communication, which can also be used for power supply. It features an onboard battery interface with charging circuit - when a 3.7V lithium battery is connected, it can be charged directly via the USB port. For battery level monitoring, the battery voltage is divided through two resistors connected to an ESP32 IO pin, allowing voltage measurement through ADC. This feature is disabled by default to reduce power consumption and can be enabled by shorting a jumper pad.

Additionally, the board includes three side-mounted buttons and a TF card interface. One button serves as the power switch, while the other two are user-definable. If you find the buttons too small for your enclosure, you can solder wires to the pads beside the buttons to connect external buttons.

## 2. Specifications

### General Parameters
- Product Type: ESP32-TFT Development Board Series
- Input Voltage: USB-5V / 3.7V-BAT (only supports 3.7V lithium batteries)
- Operating Current: Varies with code execution, typically below 100mA

### ESP32 System Parameters
- Operating Frequency: 240MHz
- Core Count: 2
- Flash Size: 16MB
- PSRAM: 8MB (current ESP32 version only supports using 4MB space)
- Antenna: Included
- Wireless: Bluetooth + WiFi 2.4G
- Screen Type: TFT IPS
- Peripherals:
  - TF card slot x1
  - User-definable buttons x2
  - 1.28" GC9A01 driver TFT screen x1
  - Battery level monitoring interface x1
- Mounting: M2 brass standoffs
- Dimensions:
  - Without card: 41mm(H) x 37mm(W)
  - With card: 41mm(H) x 44mm(W)
  - (Manual measurements may have slight variations)

## 3. Technical Details

### USB Type-C Interface
- Supports power supply, battery charging, program download, and serial communication
- For power supply, use USB 3.0 ports or phone chargers to avoid restart issues due to insufficient current
- Uses a 500mA max charging chip, limited to around 300mA by default due to size and heat considerations
- Charging current can be adjusted through resistor values:
  - 20K: 50mA
  - 10K: 100mA
  - 5K: 200mA
  - 4K: 250mA
  - 3K: 300mA
  - 2K: 400mA
  - 1.6K: 500mA

### Battery Management
- Avoid reverse polarity when connecting battery
- No built-in over-discharge protection; battery will continue discharging below 3V
- Recommend using batteries with built-in protection
- Standby current < 1μA when powered off

### Storage
- TF card support tested with 1G, 2G, and 32G capacities
- Available with or without included 2G TF card
- All units tested with video playback from TF card before shipping

### WiFi Performance
- Indoor range approximately 20 meters
- Outdoor range not tested
- Compact design necessitated some RF compromises

## 4. Important Notes
1. High integration in small form factor results in notable heat generation (normal and does not affect operation)
2. M2 standoff height is 4.5mm - screws must not exceed this length
3. For long-term battery operation, can replace the marked resistor with 69.8K to reduce heat generation (may affect USB power performance)
4. Example code provides basic functionality testing; further improvements planned

*For detailed schematics and additional technical information, please refer to the included PDF documentation.*