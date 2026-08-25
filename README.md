# Hidden Headphones

> **Bone conduction headphones that sit on your collarbone, completely hidden under your shirt.**

Hidden Headphones is an open-source hardware project for a discrete, neckband-style bone conduction audio device. The electronics sit in a small rear enclosure while flexible wiring runs through the neckband to vibration pads positioned on the collarbone. The goal is a hands-free listening device that can be worn beneath a shirt.

This repository contains the design documentation, KiCad source files, firmware, bill of materials, and OpenSCAD enclosure model for the current prototype revision.

---

## Overview & Architecture

![Design Revision v2](images/design-v2.png)

The audio path begins with a phone or computer sending Bluetooth A2DP audio to the ESP32-S3. The microcontroller outputs stereo PCM audio over I2S to the PCM5102A DAC. Two PAM8302A mono amplifiers drive the left and right bone conduction transducers installed in sealed TPU contact pads.

The project utilizes a custom electronics assembly, dedicated power subsystem, and 3D-printed housing:

* **Core processor:** ESP32-S3 Mini with Wi-Fi and Bluetooth
* **Audio input:** Bluetooth A2DP sink running on the ESP32-S3
* **Audio conversion:** PCM5102A stereo I2S DAC
* **Amplification:** Two PAM8302A 2.5 W class-D mono amplifier boards
* **Transducers:** Two 8 ohm, 1 W bone conduction transducers
* **Power:** 3.7 V, 1000 mAh LiPo battery and USB-C TP4056 charging/protection module
* **Controls:** Active-low tactile volume buttons with internal pull-ups
* **Housing:** PETG rear electronics box with flexible TPU neckband sections and sealed contact pads

### Audio and control pins

The current firmware uses the following ESP32-S3 GPIO assignments:

| Function | GPIO | Notes |
| --- | ---: | --- |
| I2S LRCK / word select | 21 | PCM5102A clock signal |
| I2S BCK / bit clock | 26 | PCM5102A clock signal |
| I2S data out | 1 | PCM5102A serial audio data |
| Volume up | 0 | Active LOW, SW1 |
| Volume down | 1 | Active LOW, SW2 |

The buttons use a 50 ms software debounce interval. A press changes the Bluetooth sink volume by 10 steps, clamped to the supported range of 0 to 127. The device advertises as **Hidden Headphones**.

## Features

* Hidden collarbone-mounted bone conduction audio
* Wireless Bluetooth A2DP playback
* Stereo output through two independent transducers
* USB-C charging through the TP4056 module
* Dedicated volume up and volume down controls
* Modular rear electronics enclosure
* Flexible cable routing through the neckband
* Open KiCad schematic and PCB source files
* Parametric OpenSCAD enclosure source

## Bill of materials

The complete sourcing list is available in [BOM.csv](BOM.csv). The main assemblies are:

| Category | Part | Quantity |
| --- | --- | ---: |
| Microcontroller | ESP32-S3 Mini development board | 1 |
| Audio DAC | PCM5102A I2S stereo module | 1 |
| Amplifier | PAM8302A mono amplifier board | 2 |
| Power management | USB-C TP4056 charging and protection module | 1 |
| Battery | 3.7 V 1000 mAh LiPo pack | 1 |
| Audio | Bone conduction transducer, 8 ohm / 1 W | 2 |
| Controls | 6 x 6 x 5 mm tactile push buttons | 3 |
| Hardware | M2 brass heat-set inserts | 4 |
| Wiring | 28 AWG stranded silicone wire | 1 set |
| Printed materials | TPU and PETG filament | 1 spool each |

## Hardware design

![Design Revision v1](images/design-v1.png)

The first design phase established the collarbone contact-pad concept, rear electronics box, battery position, and cable route. The second revision reorganized the electronics around dual amplifier channels and added space for the updated power and audio circuitry.

The rear enclosure is modeled in [rear_box.scad](rear_box.scad). Its current design includes:

* A hollow electronics cavity measuring 55 x 30 x 18 mm internally
* A 2 mm nominal wall thickness
* A USB-C charging-port cutout
* Left and right conduit openings for the neckband wiring
* Four internal PCB mounting posts
* M2 screw and heat-set insert locations
* Separate bottom-case and lid modules for 3D printing

The neckband is intended to use flexible TPU sections for comfort and PETG for the more rigid rear enclosure. The contact pads are designed around sealed TPU housings, with cable strain relief and acoustic tuning features considered in the mechanical revision.

## Firmware

The source firmware is available in [firmware/firmware.ino](firmware/firmware.ino). It uses the Arduino framework on the ESP32-S3 and the `ESP32-A2DP` and `AudioTools` libraries.

The firmware currently:

1. Starts a Bluetooth A2DP sink named `Hidden Headphones`.
2. Configures the ESP32-S3 I2S output for the PCM5102A.
3. Enables internal pull-ups for the volume buttons.
4. Debounces button input and applies volume changes.
5. Reports startup and volume changes over the serial monitor at 115200 baud.

The root-level [firmware.ino](firmware.ino) is also retained as a firmware source snapshot. Keep the two copies synchronized when making firmware changes.

### Flashing and verification

1. Install the ESP32 Arduino board support package or configure an equivalent PlatformIO ESP32-S3 environment.
2. Install the `ESP32-A2DP` and `AudioTools` libraries.
3. Select the connected ESP32-S3 board and its USB serial port.
4. Flash `firmware/firmware.ino`.
5. Open a serial monitor at 115200 baud.
6. Confirm the `Bluetooth A2DP Sink ready` message.
7. Pair a phone or computer with **Hidden Headphones**.
8. Verify left and right audio output and test both volume buttons.

The pin assignments should be checked against the assembled PCB before powering the complete system. GPIO 1 is used for both I2S data output and the volume-down button in the current firmware, so the final hardware revision must confirm that this multiplexing is intentional and electrically compatible.

## Repository structure

```text
.
├── BOM.csv                                      # Component list and sourcing links
├── JOURNAL.md                                   # Project development log and time tracking
├── LICENSE                                      # CC0-1.0 license
├── README.md                                    # Project overview and build guide
├── firmware.ino                                 # Root firmware snapshot
├── firmware/firmware.ino                        # Working firmware source
├── rear_box.scad                                # Parametric rear enclosure model
├── Hidden_Headphones_All_Components_v3.kicad_sch # KiCad schematic
├── Hidden_Headphones_All_Components_v3.kicad_pcb # KiCad PCB layout
├── Hidden_Headphones_All_Components_v3.kicad_pro # KiCad project settings
├── images/                                      # Design, CAD, PCB, and firmware images
└── Untitled/                                    # Additional KiCad working files
```

## Project history

The development log in [JOURNAL.md](JOURNAL.md) records the design process and time spent on each stage:

* **Initial concept:** Established the ESP32-S3 audio architecture, collarbone contact pads, TPU housing, and USB-C rechargeable battery system.
* **CAD and assembly:** Designed the rear electronics box, internal mounting layout, cable conduits, and labyrinth-style strain relief.
* **Component sourcing:** Selected the controller, DAC, amplifiers, charging module, battery, transducers, controls, hardware, wiring, and print materials.
* **Architecture revision:** Added dual PAM8302A amplifiers, revised the power system, and documented the antenna keep-out and sealed contact-pad concepts.
* **Housing model:** Completed the main swept and hollowed enclosure and aligned internal hardware mounting points.
* **PCB design:** Created the KiCad schematic and board layout, resolved schematic and footprint issues, and prepared the design for fabrication.
* **Firmware:** Implemented Bluetooth A2DP playback, I2S routing, volume controls, debouncing, serial output, and the flashing verification procedure.

## Current status

The repository represents a prototype design prepared for hardware fabrication and bring-up. The PCB, firmware, and enclosure should be validated together on assembled hardware, especially the power connections, GPIO assignments, amplifier wiring, battery protection, enclosure clearances, and transducer sealing.

This is an experimental hardware project. Use an appropriate LiPo charging module, inspect batteries and wiring for damage, and do not leave the device charging unattended.

---