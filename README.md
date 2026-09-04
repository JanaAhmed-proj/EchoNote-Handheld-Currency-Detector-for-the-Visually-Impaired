*Project Report: Handheld Multi-Sensory Currency Detector**

**1. Introduction**

The objective of this project is to design and develop a portable, handheld device capable of identifying currency & its value. This device is specifically engineered to assist the visually impaired and commercial handlers by providing both visual and auditory feedback.

### 2. System Architecture & Components

The device follows a modular design to ensure portability and efficiency. The integration of image processing with audio synthesis allows for a "multi-sensory" user experience.

#### A. Image Acquisition (The Camera)

- **Component:** CMOS Camera Module (e.g.,ESP32-CAM).

- **Role:** The camera acts as the primary sensor. It captures a frame of the banknote.

#### B. Central Processing (The Microcontroller)

- **Component:** High-speed Microcontroller (e.g.,ESP32).

- **Role:** The "brain" of the device. It performs the following tasks:

  1.  **Image Pre-processing:** Converts images to grayscale and resizes them.

  2.  **Pattern Matching:** Compares the captured data against stored templates or a lightweight Neural Network.

  3.  **Instruction Routing:** Sends the result to both the display and the audio module simultaneously.

#### C. Visual Feedback (The LCD)

- **Component:** TFT LCD Display.

- **Role:** Provides a clear visual readout of the currency value (e.g., "$50 USD") and the system status (e.g., "Scanning")

- **Design:** High-contrast text is used to ensure readability for users with low vision.

#### D. Auditory Feedback (MP3 Module & Speaker)

- **Components:** DFPlayer Mini (MP3 Module) + 8-ohm Micro Speaker.

- **Role:** To make the device accessible, an MP3 module stores pre-recorded voice clips for each denomination.

### 3. Power Management & Portability

To ensure the device is truly handheld and practical for daily use, a robust power circuit is integrated:

- **Energy Source:** A **Lithium-Ion (Li-ion) Battery** (e.g., 18650 or Li-Po) provides the high current required for image processing.

- **Charging System:** A **TP4056 Charger Module** with a USB-C or Micro-USB port allows the user to recharge the device using standard mobile chargers.

- **User Control:** A physical **Toggle Switch** or momentary push-button manages the power state, preventing parasitic battery drain when the device is in a pocket or bag.

<table>
<colgroup>
<col style="width: 22%" />
<col style="width: 28%" />
<col style="width: 19%" />
<col style="width: 29%" />
</colgroup>
<thead>
<tr class="header">
<th>Item No.</th>
<th>Item</th>
<th>Price<br />
in EGP</th>
<th>Seller</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>1</td>
<td>ESP32 Camera</td>
<td>510</td>
<td>Free Electronics</td>
</tr>
<tr class="even">
<td>2</td>
<td>Perfboard</td>
<td>25</td>
<td>Free Electronics</td>
</tr>
<tr class="odd">
<td>3</td>
<td>Solder wire</td>
<td>120</td>
<td>Free Electronics</td>
</tr>
<tr class="even">
<td>4</td>
<td>IPS Screen 240x240 1.3 inch</td>
<td>300</td>
<td>Free Electronics</td>
</tr>
<tr class="odd">
<td>5</td>
<td>Switch</td>
<td>2.5</td>
<td>Free Electronics</td>
</tr>
<tr class="even">
<td>6</td>
<td>Battery</td>
<td>120</td>
<td>Free Electronics</td>
</tr>
<tr class="odd">
<td>7</td>
<td>Battery charger TP4056</td>
<td>30</td>
<td>Free Electronics</td>
</tr>
<tr class="even">
<td>9</td>
<td>MP3 TP16 Module</td>
<td>120</td>
<td>Free Electronics</td>
</tr>
<tr class="odd">
<td>10</td>
<td>Speaker 8 ohm</td>
<td>12</td>
<td>Free Electronics</td>
</tr>
<tr class="even">
<td>11</td>
<td>Wires &amp; Headers</td>
<td>50</td>
<td>Free Electronics</td>
</tr>
</tbody>
</table>

**Description of each component:**

![image1.jpeg](assets/image1.jpeg)**1- ESP32 Camera:**

It is an esp32 which is a very famous microcontroller with Wi-Fi and Bluetooth capability also with high clock frequency which is used in processing paired with a camera module used in computer vision applications.

Its main role here is to capture and process images also to broadcast visuals to the LCD and speech to the mp3 module.

**2- Perfboard:**

It is a breadboard alternative but used for more permanent connection by means of soldering.![image2.jpg](assets/image2.jpg)

**3- Solder wire:**

It is used to bond wires with the perfboard.

![image3.jpeg](assets/image3.jpeg)

**4- IPS Screen 240x240 1.3 inch:**

It is used for visuals in this project paired with a microcontroller to handle the graphics and it communicates with the esp32 by means of SPI.

![image4.jpg](assets/image4.jpg)

**5- Switch:**

It is used to complete the circuit upon switching on.

![image5.jpeg](assets/image5.jpeg)

**6- Battery:**

**it is used as a power source.**

![image6.jpg](assets/image6.jpg)

**7- Battery charger TP4056:**

Used for charging the battery.

![image7.jpg](assets/image7.jpg)

**8- MP3 TP16 Module:**

It is used for speaker output to read the currency detection output.

![image8.jpg](assets/image8.jpg)

**9- Speaker 8 ohm:**

Used to output sound.

![image9.jpg](assets/image9.jpg)

**10- Wires & Headers:**

Used for connection between components.

![image10.jpg](assets/image10.jpg)

![image11.jpg](assets/image11.jpg)

![image12.PNG](assets/image12.PNG)**Wiring Diagram:**
