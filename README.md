# STM32 Water Level Monitoring System

ADC-based liquid level measurement system using **STM32**, **SSD1306 OLED**, and **ADC with DMA**.

This project is developed as an embedded monitoring system for measuring and displaying liquid level information using an STM32 microcontroller.

> **Note:** This project is shared for learning purposes. It is licensed under the **GNU GPL v3** because it includes a third-party SSD1306 display driver released under that license — see the [License](#license) and [Third-Party Components](#third-party-components) sections below.

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
| Microcontroller         | STM32               |
| Display                 | SSD1306 OLED        |
| ADC                     | STM32 Internal ADC  |
| Communication           | I2C                 |
| Programming Environment | STM32CubeIDE        |
| Firmware Library        | STM32 HAL           |

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

## Third-Party Components

This project uses a modified SSD1306 OLED display driver (`Drivers/SSD1306/ssd1306.c`, `ssd1306.h`, `fonts.c`, `fonts.h`) and also  originally written by:

* **Tilen Majerle** ([tilen@majerle.eu](mailto:tilen@majerle.eu)) — original author
* **Alexander Lutsai** ([s.lyra@ya.ru](mailto:s.lyra@ya.ru)) — STM32F10x port/modification

These files are licensed under the **GNU General Public License v3 (or later)**, as stated in their original file headers. They have been lightly reformatted (indentation, comments, dead-code removal) for this project, but the license and attribution have been kept intact, as required by the GPL.

Because this driver is compiled together with the rest of the firmware into a single combined work, **the whole project is distributed under GPL v3** — see [License](#license) below.

## Author

**Kurnia Aditya Reynaldi**

Electrical Engineer | Embedded Systems | Control Systems

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

It was originally intended to be MIT-licensed, but because it includes and links against the third-party SSD1306 driver (`ssd1306.c`/`fonts.c`) under GPL v3, the entire combined firmware must also be distributed under GPL v3 terms. Application-specific code written for this project (`main.c`, `main.h`) is authored by Kurnia Aditya Reynaldi and is released as part of this GPL-3.0-licensed project.

See the [LICENSE](LICENSE) file for the complete license text.

```text
STM32 Water Level Monitoring System
Copyright (C) 2024  Kurnia Aditya Reynaldi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
```
