# MoodEye Hardware Architecture

## Power System
The device is powered by a 3.7V battery system with the following components:
- Battery charging circuit with USB-C input
- Single-key power switch for on/off control
- Boost converter for voltage step-up
- LDO_3V3 regulator for stable 3.3V output
- Power path includes protective diodes
- Pull-down resistors: 100K and 400K for power control

## Communication Interface
- USB-Type C connector for power and data
- USB-TTL Download interface with:
  - TX/RX lines for serial communication
  - RST line for system reset
  - Standard USB protocol pins (IO0, TXD, RXD)

## Core Components

### ESP32 Mini System
- Main microcontroller for system control
- Pin assignments:
  - IO32-IO44: Display interface
  - SDA/SCL: I2C communication
  - CS/BLK: Chip select and control signals
  - 3V3/GND: Power rails
  - Additional GPIO pins for sensor interfaces

### Display Module
- Model: 1.28" TFT GC9A01
- Interface pins:
  - CS: Chip select
  - DC: Data/Command control
  - RES: Reset
  - SDA: Serial data
  - SCL: Serial clock
  - BLK: Backlight control
  - 3V3/GND: Power supply

### Switch Interface
- Dedicated switch module with GND reference
- Switch pins (SW1, SW2) for user input
- Tied to ESP32 GPIO pins for interrupt handling

## Power Management
The system implements a multi-stage power management approach:
1. Battery power (3.7V) input
2. Charging circuit for battery maintenance
3. Boost conversion for voltage regulation
4. Final 3.3V LDO regulation for clean power to digital components

## Connection Notes
- All digital communication lines are referenced to system GND
- Power rails are properly decoupled with appropriate bypass capacitors
- Signal integrity maintained through proper routing and pin assignments
- Hardware supports the full feature set outlined in the project requirements