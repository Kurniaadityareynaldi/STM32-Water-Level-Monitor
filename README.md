# STM32 Water Level Monitoring System

ADC-based liquid level measurement system using **STM32**, **SSD1306 OLED**, and **ADC with DMA**.

This project is developed as an embedded monitoring system for measuring and displaying liquid level information using an STM32 microcontroller.

## Features

* ADC-based liquid level measurement
* ADC data acquisition using DMA
* Real-time level calculation
* Liquid level percentage monitoring
* Level measurement in centimeters
* OLED display using SSD1306
* I2C communication
* Timer-based processing
* STM32 HAL firmware architecture
* Developed using STM32CubeIDE

## Hardware

| Component               | Description        |
| ----------------------- | ------------------ |
| Microcontroller         | STM32              |
| Display                 | SSD1306 OLED       |
| ADC                     | STM32 Internal ADC |
| Communication           | I2C                |
| Programming Environment | STM32CubeIDE       |
| Firmware Library        | STM32 HAL          |

## System Overview

The system reads an analog signal from the liquid-level sensor through the STM32 ADC.

The ADC value is processed by the firmware and converted into:

1. ADC measurement value
2. Liquid level percentage
3. Estimated liquid height in centimeters

The resulting information is displayed on an SSD1306 OLED.

```text
Liquid Level Sensor
        │
        ▼
   STM32 ADC + DMA
        │
        ▼
 Level Calculation
        │
        ├── Percentage
        ├── ADC Value
        └── Level (cm)
        │
        ▼
   SSD1306 OLED
```

## Firmware

The firmware is based on the **STM32 HAL (Hardware Abstraction Layer)** and generated/configured using **STM32CubeIDE / STM32CubeMX**.

### Main peripherals

* **ADC1**

  * Continuous conversion
  * DMA data acquisition
  * Analog input measurement

* **I2C1**

  * OLED communication
  * SSD1306 display

* **TIM2**

  * Periodic timer interrupt

* **TIM3**

  * Periodic timer interrupt

* **DMA**

  * Automatic ADC data transfer

## Level Calculation

The firmware converts the ADC reading into a percentage based on the configured ADC range.

The measured level is then converted into an estimated height using a calibration/correction factor.

The current firmware uses:

```c
#define ADC_MIN_VALUE           0U
#define ADC_MAX_VALUE           250U
#define LEVEL_CORRECTION_FACTOR 0.473f
```

These values are calibration parameters and may need to be adjusted according to the sensor, tank/container dimensions, and measurement characteristics.

## OLED Display

The OLED provides the following information:

* Liquid level condition
* Level percentage
* ADC value
* Estimated liquid height

Example display information:

```text
Kondisi
ADC: 125
Ketinggian: 59.1 cm
50%
```

The displayed percentage is categorized into level thresholds such as:

* 0%
* 25%
* 50%
* 75%
* 100%

## Project Structure

A recommended repository structure is:

```text
stm32-water-level-monitor/
│
├── Core/
│   ├── Inc/
│   │   └── main.h
│   │
│   └── Src/
│       └── main.c
│
├── Drivers/
│   └── SSD1306/
│       ├── ssd1306.c
│       ├── ssd1306.h
│       ├── fonts.c
│       └── fonts.h
│
├── stm32-water-level-monitor.ioc
│
├── README.md
├── LICENSE
└── .gitignore
```

> The exact folder structure may vary depending on the STM32CubeIDE project configuration.

## Development Environment

* **IDE:** STM32CubeIDE
* **Framework:** STM32 HAL
* **Language:** C
* **Configuration:** STM32CubeMX / `.ioc`
* **Display Driver:** SSD1306
* **Communication:** I2C
* **Data Acquisition:** ADC + DMA

## Calibration

The level measurement should be calibrated according to the actual mechanical installation and sensor characteristics.

The following parameters are intended to be adjusted during calibration:

```c
ADC_MIN_VALUE
ADC_MAX_VALUE
LEVEL_CORRECTION_FACTOR
```

For a different sensor or tank/container size, these parameters may need to be recalculated.

## Future Development

Possible improvements include:

* Automatic sensor calibration
* Moving-average filtering
* Sensor fault detection
* Low-level and high-level alarms
* Relay/pump control
* Data logging
* UART monitoring
* CAN communication
* IoT connectivity
* Improved modular firmware architecture

## Author

**Kurnia Aditya Reynaldi**

Electrical Engineer | Embedded Systems | Control Systems

## License

This project is licensed under the **MIT License**.

See the [LICENSE](LICENSE) file for the complete license text.
