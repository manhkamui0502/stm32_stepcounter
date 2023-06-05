# STM32F103C8T6 _ STEP COUNTER (ELT3240 1)
## Introduction
Bài tập lớn cuối kỳ - Step Counter using STM32F103C8T6 with MPU6050 and OLED display 

## Contributors
- Trần Đức Mạnh - 20020230 - K65K
- Lê Tuấn Minh  - 20020694 - K65K

## Hardware 
- STM32F103C8T6 "Blue Pill" 
- 0.96' OLED display 
- MPU 6050
- 10440 Battery x2
- BreadBoard
- Traffic light LED
- Buck 3A MP1584EN Mini
- 3D printed mount
- 2 button (Reset and Pause)

## Systems
- Build with Keil C

## Features
- Counting step base on processed data that collected from MPU6050 and display.
- Green LED Blink at 1Hz while state is Running.
- While pause, Red LED is high.
- Pause button => switch state between Running and Pause.
- Reset button => Reset steps to 0.

### I2C Interface
- Using I2C interface to display data to OLED and Read data from MPU6050.

### TIMER2
- Using TIM2 to blink Green LED 

### GPIO Input and Output
- Using 2 Input Switch
- Using Red LED and Blue LED to show current status

### Algorithm idea to detect step
- Let sample rate is 15 per read
- Datas from MPU6050 include x, y, z accelerate but there was so much noise 
=> solution is using a simple kalman filter, then we have x_final[i], y_final[i], z_final[i].
- Next, finding magnitude[i] = sprt(x_final[i]*x_final[i] + y_final[i]*y_final[i] + z_final[i]*z_final[i])
- Then find sample Mean and Sample Standard Deviation of magnitude[].
- Define static threshold = 3, dynamic threashold = Standard Deviation * static threshold + 9 (base on your own data)
- If magnitude[i] is maximum in sample, and magnitude[i] - mean > dynamic threshold => detect a step.

#### Some picture
- Running 
![348361134_710521987505572_8976193415910899223_n](https://github.com/manhkamui0502/stm32_stepcounter/assets/92159614/d776d253-4a9c-4f0b-bfb1-89eebb208754)
- Pause
![352093601_570339658559724_456006455266362022_n](https://github.com/manhkamui0502/stm32_stepcounter/assets/92159614/73493aa4-2d46-4c64-8ff7-8e210a821da4)
