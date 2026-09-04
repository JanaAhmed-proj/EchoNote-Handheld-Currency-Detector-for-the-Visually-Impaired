# EchoNote — Handheld Currency Detector for the Visually Impaired

A portable ESP32-CAM device that identifies banknotes in real time and announces their value aloud. Combines on-device image classification, a TFT display for sighted users, and a DFPlayer Mini audio module for spoken feedback — built as an affordable multi-sensory assistive tool for visually impaired and commercial cash handlers.
For the complete project files, organized data, and additional materials not available on GitHub, please visit this link below 👇🏻.
https://drive.google.com/drive/folders/1r5StB7k9eh6it5cfRXF75wriorMYNs-b



## Table of Contents

- [Introduction](#introduction)
- [System Architecture](#system-architecture)
  - [Image Acquisition — Camera](#a-image-acquisition-the-camera)
  - [Central Processing — Microcontroller](#b-central-processing-the-microcontroller)
  - [Visual Feedback — LCD](#c-visual-feedback-the-lcd)
  - [Auditory Feedback — MP3 Module & Speaker](#d-auditory-feedback-mp3-module--speaker)
- [Power Management & Portability](#power-management--portability)
- [Bill of Materials](#bill-of-materials)
- [Firmware](#firmware)

## Introduction

The objective of this project is to design and develop a portable, handheld device capable of identifying currency and its value. This device is specifically engineered to assist the visually impaired and commercial handlers by providing both visual and auditory feedback.

## System Architecture

The device follows a modular design to ensure portability and efficiency. The integration of image processing with audio synthesis allows for a "multi-sensory" user experience.

### A. Image Acquisition (The Camera)

| | |
|---|---|
| **Component** | CMOS Camera Module (e.g., ESP32-CAM) |
| **Role** | Primary sensor — captures a frame of the banknote |

### B. Central Processing (The Microcontroller)

| | |
|---|---|
| **Component** | High-speed Microcontroller (e.g., ESP32) |
| **Role** | The "brain" of the device |

It performs the following tasks:

1. **Image Pre-processing** — Converts images to grayscale and resizes them.
2. **Pattern Matching** — Compares the captured data against stored templates or a lightweight Neural Network.
3. **Instruction Routing** — Sends the result to both the display and the audio module simultaneously.

### C. Visual Feedback (The LCD)

| | |
|---|---|
| **Component** | TFT LCD Display |
| **Role** | Clear visual readout of the currency value (e.g., "$50 USD") and system status (e.g., "Scanning") |
| **Design** | High-contrast text is used to ensure readability for users with low vision |

### D. Auditory Feedback (MP3 Module & Speaker)

| | |
|---|---|
| **Components** | DFPlayer Mini (MP3 Module) + 8-ohm Micro Speaker |
| **Role** | An MP3 module stores pre-recorded voice clips for each denomination, making the device accessible |

## Power Management & Portability

To ensure the device is truly handheld and practical for daily use, a robust power circuit is integrated:

- **Energy Source** — A Lithium-Ion (Li-ion) Battery (e.g., 18650 or Li-Po) provides the high current required for image processing.
- **Charging System** — A TP4056 Charger Module with a USB-C or Micro-USB port allows the user to recharge the device using standard mobile chargers.
- **User Control** — A physical Toggle Switch or momentary push-button manages the power state, preventing parasitic battery drain when the device is in a pocket or bag.

## Bill of Materials

| Item No. | Item | Price (EGP) | Seller |
|---|---|---|---|
| 1 | ESP32 Camera | 510 | Free Electronics |
| 2 | Perfboard | 25 | Free Electronics |
| 3 | Solder wire | 120 | Free Electronics |
| 4 | IPS Screen 240x240 1.3 inch | 300 | Free Electronics |
| 5 | Switch | 2.5 | Free Electronics |
| 6 | Battery | 120 | Free Electronics |
| 7 | Battery charger TP4056 | 30 | Free Electronics |
| 9 | MP3 TP16 Module | 120 | Free Electronics |
| 10 | Speaker 8 ohm | 12 | Free Electronics |
| 11 | Wires & Headers | 50 | Free Electronics |

### Component Notes

- **ESP32 Camera** — A famous Wi-Fi/Bluetooth-capable microcontroller with a high clock frequency, paired with a camera module for computer vision. Captures and processes images, and broadcasts visuals to the LCD and speech to the MP3 module.
- **Perfboard** — A breadboard alternative used for more permanent connections by means of soldering.
- **Solder wire** — Used to bond wires to the perfboard.
- **IPS Screen 240x240 1.3"** — Handles the visual output, communicating with the ESP32 via SPI.
- **Switch** — Completes the circuit upon power-on.
- **Battery** — Used as the power source.
- **Battery charger TP4056** — Used for charging the battery.
- **MP3 TP16 Module** — Drives speaker output to read out the currency detection result.
- **Speaker 8 ohm** — Outputs sound.
- **Wires & Headers** — Used for connections between components.

## Firmware

The firmware (`main_improved2.ino`) handles camera capture, on-device inference (via Edge Impulse), TFT display rendering, and DFPlayer Mini audio playback for detected denominations.

all data for this project is provided in this link drive 
https://drive.google.com/drive/folders/1r5StB7k9eh6it5cfRXF75wriorMYNs-b
