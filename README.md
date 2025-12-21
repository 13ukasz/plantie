# Plantie

A smart plant watering system based on ESP32 that monitors soil moisture and temperature, automatically activates a water pump when needed, and displays sensor data on a web server.

## Features

- **Automatic Watering**: Measures soil moisture and automatically triggers water pump when moisture is too low
- **Temperature Monitoring**: Reads ambient temperature via DS18B20 sensor
- **Web Interface**: View sensor parameters on an HTML dashboard
- **Zigbee Control**: Optional remote pump control via Zigbee (can be connected to home assistants like Alexa)

## Components

- **Moisture Sensor**: ADC-based soil moisture reading
- **Temperature Sensor**: DS18B20 1-Wire temperature sensor
- **Relay Control**: GPIO-based water pump relay control
- **Web Server**: HTML interface for monitoring
- **Zigbee Relay** (optional): Remote control integration

## Getting Started

### Build and Flash

To compile using ESP-IDF you first need to setup the toolchain as described [here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html).

```bash
idf.py set-target esp32c6
idf.py build
idf.py flash
```

## Hardware

- ESP32 Development Board
- Soil Moisture Sensor (ADC)
- DS18B20 Temperature Sensor (GPIO 1)
- Relay Module (GPIO 4)
- Water Pump

## License

This project uses ESP-IDF and Espressif libraries.