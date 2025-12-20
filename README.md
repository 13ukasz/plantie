# Plantie

An ESP32-based smart plant watering system that monitors soil moisture and temperature, and automatically controls a water pump via a relay. The system integrates Zigbee connectivity for remote control.

## Overview

Plantie is a plant care automation system designed for IoT enthusiasts. It features:

- **Moisture Monitoring**: Continuously reads soil moisture using an ADC-connected moisture sensor and automatically triggers a water pump when moisture levels drop below a threshold
- **Temperature Sensing**: Monitors ambient temperature using a DS18B20 digital temperature sensor connected via 1-Wire protocol
- **Smart Relay Control**: Controls a water pump relay via GPIO, with automatic shutoff after a specified duration
- **Zigbee Integration**: Enables remote control and monitoring of the water pump through Zigbee mesh networks

## Hardware Requirements

- **ESP32 Development Board**
- **Soil Moisture Sensor** (analog output, connected to ADC1_CHANNEL_0)
- **DS18B20 Temperature Sensor** (1-Wire protocol, connected to GPIO 1)
- **Relay Module** (for water pump control, connected to GPIO 4)
- **Water Pump** (controlled by relay)
- **Appropriate wiring and power supplies**

## Project Structure

```
plantie/
├── main/
│   ├── main.c              # Application entry point
│   └── CMakeLists.txt
├── components/
│   ├── moisture_sensor/    # Moisture sensing component
│   │   ├── moisture_sensor.c
│   │   ├── moisture_sensor.h
│   │   └── CMakeLists.txt
│   ├── temperature_sensor/ # DS18B20 temperature component
│   │   ├── temperature_sensor.c
│   │   ├── temperature_sensor.h
│   │   └── CMakeLists.txt
│   └── zigbee_relay/       # Zigbee relay control component
│       ├── zigbee_relay.c
│       ├── zigbee_relay.h
│       └── CMakeLists.txt
├── CMakeLists.txt
└── README.md
```

## Components Description

### Moisture Sensor Component
- Reads soil moisture via ADC (12-bit resolution, 0-2.5V range)
- Converts ADC values to voltage readings
- Automatically triggers the water pump when moisture voltage drops below 1.2V
- Pump runs for 2 seconds per activation
- Logs readings every 1 second

**Configuration:**
- ADC Channel: ADC1_CHANNEL_0
- Relay GPIO: 4
- Moisture Threshold: 1.2V
- Pump Duration: 2000ms
- Read Interval: 1000ms

### Temperature Sensor Component
- Initializes and reads from DS18B20 temperature sensor
- Uses 1-Wire protocol for communication
- Provides temperature readings in Celsius
- Read interval: 1000ms

**Configuration:**
- 1-Wire GPIO: 1
- Sensor Type: DS18B20

### Zigbee Relay Component
- Integrates Zigbee Home Automation (ZHA) standard
- Allows remote control of the water pump via Zigbee commands
- Supports network commissioning and joining
- Responds to ON/OFF cluster commands on endpoint 10
- Can be controlled through Zigbee coordinators and home automation systems

**Configuration:**
- Relay GPIO: 4
- Relay Duration: 5000ms
- Endpoint: 10
- Role: Zigbee End Device (ZED)
- Keep Alive: 3000ms

## Setup Instructions

### Prerequisites

- ESP-IDF 5.0 or later installed
- ESP32 board connected via USB-to-Serial adapter
- Compatible serial port (e.g., `/dev/ttyUSB0` on Linux/Mac, `COM3` on Windows)

### 1. **Configure the Project**

```bash
cd /path/to/plantie
idf.py set-target esp32
idf.py menuconfig
```

Configure Zigbee settings in `Component config → Zigbee → Zigbee mode` as needed.

### 2. **Build the Project**

```bash
idf.py build
```

### 3. **Flash to ESP32**

```bash
idf.py -p /dev/ttyUSB0 flash
```

Replace `/dev/ttyUSB0` with your serial port:
- **Linux/Mac**: `/dev/ttyUSB0`, `/dev/ttyUSB1`, etc., or `/dev/cu.SLAB_USBtoUART`
- **Windows**: `COM3`, `COM4`, etc.

### 4. **Monitor Output**

```bash
idf.py -p /dev/ttyUSB0 monitor
```

You should see log output from the moisture sensor, temperature sensor, and Zigbee components.

## Operation

### Automatic Watering

1. The moisture sensor continuously monitors soil moisture every 1 second
2. When moisture voltage drops below 1.2V, the pump automatically activates for 2 seconds
3. The system waits before checking again to avoid rapid retriggering

### Remote Control

1. Join the ESP32 to a Zigbee network through network steering
2. Once connected, control the water pump through Zigbee commands (ON/OFF cluster)
3. Monitor the device's network status and Extended PAN ID in the logs

### Temperature Monitoring

1. The DS18B20 sensor reads temperature every 1 second
2. Readings are logged to the console for monitoring and debugging

## GPIO Pin Assignments

| GPIO | Function | Purpose |
|------|----------|---------|
| 0 | ADC1_CHANNEL_0 | Moisture Sensor Input |
| 1 | 1-Wire Bus | DS18B20 Temperature Sensor |
| 4 | GPIO Output | Relay Control (Water Pump) |

## Troubleshooting

### No ADC Readings
- Check moisture sensor wiring to GPIO 0
- Verify sensor is providing analog voltage output (0-3.3V)
- Check ADC configuration in `moisture_sensor.c`

### Temperature Sensor Not Found
- Verify DS18B20 is connected to GPIO 1 via 1-Wire protocol
- Check for proper pull-up resistor (typically 4.7kΩ)
- Ensure correct wiring and power supply

### Zigbee Not Connecting
- Ensure Zigbee coordinator is available
- Check Extended PAN ID and channel in logs
- Verify antenna connection (if external antenna is used)
- Factory reset may be needed: Check logs for "Device started up in factory-reset mode"

### Pump Not Activating
- Verify relay is connected to GPIO 4
- Check relay and pump power supply
- Confirm moisture threshold (1.2V) is being reached
- Monitor ADC voltage readings in logs

## Future Enhancements

- Web dashboard for monitoring plant health
- Plant-specific watering schedules
- Multiple plant support with individual zones
- Cloud integration for historical data storage
- Mobile app for remote monitoring and control

## License

This project uses ESP-IDF and Espressif libraries.