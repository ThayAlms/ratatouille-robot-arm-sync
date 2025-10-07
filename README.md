# ratatouille-robot-arm-sync
A Ratatouille-inspired robot rat that mimics your arm movements in real time — powered by Arduino magic and a dash of maker passion. 🐭⚙️
![Python](https://img.shields.io/badge/python-3.x-blue.svg)
![Arduino](https://img.shields.io/badge/arduino-uno-red.svg)
![MediaPipe](https://img.shields.io/badge/mediapipe-latest-orange.svg)



# 🧀🤖 RemyBot — The Little Rat That Moves Like You

> “Not everyone can become a great chef, but a great chef can come from anywhere… even from an Arduino.”

RemyBot is a motion-controlled robotic rat inspired by **Remy**, the brilliant little chef from *Ratatouille*.  
By strapping **MPU6050 sensors to your wrists**, you can make Remy **move his left and right arms** in perfect sync with your own. 🐭✨

It’s like having your very own Pixar-style puppet — only smarter, cuter, and built with solder and code.

---

## 🎬 Project Overview

RemyBot connects your movements to his:
- You lift your left arm → his left arm lifts.  
- You twist your wrist → his paw follows.  
- You cheer “Oui, chef!” → both arms raise in victory. 🇫🇷  

Using **two MPU6050 motion sensors** (one per wrist) and **two servo motors** (one per arm), the Arduino reads your gestures and brings the rat to life.

---

## ⚙️ How It Works

Each **MPU6050** measures your wrist’s **tilt and rotation** in real time.  
The **Arduino UNO** reads both sensors via I2C, applies a **complementary filter** to smooth the data, and maps those angles to **two servos** that move Remy’s arms.

The result: fluid, expressive, and natural motion — as if Remy is truly mimicking his human chef.

---

## 🧩 Components

| Component | Qty | Description |
|------------|-----|-------------|
| Arduino UNO | 1 | The main controller |
| MPU6050 (GY-521) | 2 | One for each wrist |
| SG90 Servo Motor | 2 | Left and right arms |
| Jumper wires | Several | For connections |
| Rat model / puppet | 1 | Your very own Remy |
| External 5V supply | 1 | Recommended for servos |

---

## 🔌 Wiring Overview

| MPU6050 | Arduino | Function |
|----------|----------|-----------|
| VCC | 5V | Power |
| GND | GND | Ground |
| SDA | A4 | I2C data |
| SCL | A5 | I2C clock |

| Servo | Pin | Description |
|--------|------|-------------|
| Left Arm Servo | 9 | Moves the left arm |
| Right Arm Servo | 10 | Moves the right arm |

> 💡 Tip: Use an external 5V power source to avoid jitter — Remy doesn’t like low voltage before dinner.

---

## 🧠 The Code

The sketch fuses data from both MPU6050 sensors using a **complementary filter**, balancing the accelerometer’s stability and the gyroscope’s responsiveness.

The filtered pitch/roll angles are mapped to each servo’s motion range.  
You can fine-tune direction and offset with:

```cpp
invertServo[]     // reverses servo direction
offsetServoDeg[]  // corrects misalignment

