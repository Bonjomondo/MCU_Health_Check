# 系统架构图

## 硬件连接图

```
STM32F103C8T6
    |
    |-- PB10 (I2C_SCL) ----+---- OLED
    |-- PB11 (I2C_SDA) ----+---- MAX30102
                           +---- MPU6050
                           +---- SHT30
    |
    |-- PA1 (One-Wire) --------- DS18B20
    |
    |-- PA4 (SPI_CS)   ----+
    |-- PA5 (SPI_SCK)  ----+---- W25Q64 Flash
    |-- PA6 (SPI_MISO) ----+
    |-- PA7 (SPI_MOSI) ----+
    |
    |-- PA9  (USART1_TX) --+---- HC-05
    |-- PA10 (USART1_RX) --+     Bluetooth
    |
    |-- PA2 (GPIO) --------- Motor (震动马达)
    |-- PA3 (GPIO) --------- Buzzer (蜂鸣器)
    |
    |-- PC13 (GPIO_IN) ----- KEY1
    |-- PC14 (GPIO_IN) ----- KEY2
    |-- PC15 (GPIO_IN) ----- KEY3
    |-- PA0  (GPIO_IN) ----- KEY4
```

## 软件架构图

```
+------------------+
|   Application    |
|    (main.c)      |
+------------------+
        |
        |
+------------------+
|   Task Layer     |
|   8 Tasks with   |
|   Scheduler      |
+------------------+
        |
        +-- Task_HeartRateSpO2 (50ms)
        +-- Task_Temperature (1000ms)
        +-- Task_MotionDetect (100ms)
        +-- Task_Environment (2000ms)
        +-- Task_IdleAlert (10000ms)
        +-- Task_DataTransmit (1000ms)
        +-- Task_DisplayUpdate (200ms)
        +-- Task_KeyScan (10ms)
        |
+------------------+
|  Driver Layer    |
+------------------+
        |
        +-- Sensor Drivers
        |   +-- MAX30102
        |   +-- DS18B20
        |   +-- MPU6050
        |   +-- SHT30
        |
        +-- Peripheral Drivers
        |   +-- OLED
        |   +-- Key
        |   +-- Buzzer
        |   +-- Motor
        |   +-- W25Q64
        |   +-- Serial
        |
        +-- Communication Protocol
            +-- MyI2C
            +-- MySPI
            +-- One-Wire (in DS18B20)
            +-- USART (Serial)
```

## 数据流程图

```
                      +-------------+
                      | Sensors     |
                      +-------------+
                            |
        +-------------------+-------------------+
        |                   |                   |
        v                   v                   v
  MAX30102            MPU6050/SHT30        DS18B20
  (HR/SpO2)           (Motion/Env)         (Temp)
        |                   |                   |
        +-------------------+-------------------+
                            |
                            v
                   +----------------+
                   | Data Process   |
                   | (HealthAlgo)   |
                   +----------------+
                            |
        +-------------------+-------------------+
        |                   |                   |
        v                   v                   v
    +-------+          +---------+         +--------+
    | OLED  |          | Buzzer/ |         | Flash  |
    | Display|         | Motor   |         | Storage|
    +-------+          +---------+         +--------+
                            |
                            v
                      +----------+
                      | Bluetooth|
                      | (HC-05)  |
                      +----------+
                            |
                            v
                      [ Mobile App ]
```

## 任务调度时序图

```
Time (ms) | Tasks Running
----------|--------------------------------------------------------------
0         | KeyScan
10        | KeyScan
20        | KeyScan
30        | KeyScan
40        | KeyScan
50        | KeyScan + HeartRateSpO2
60        | KeyScan
70        | KeyScan
80        | KeyScan
90        | KeyScan
100       | KeyScan + MotionDetect
...       | ...
200       | KeyScan + DisplayUpdate
...       | ...
1000      | KeyScan + Temperature + DataTransmit
...       | ...
2000      | KeyScan + Environment
...       | ...
10000     | KeyScan + IdleAlert
```

## 状态机图

```
+------------+
|   Power On |
+------------+
      |
      v
+------------+
| Initialize |
| - Sensors  |
| - Display  |
| - Scheduler|
+------------+
      |
      v
+------------+     KEY2      +-----------+
|  Running   | <-----------> |  Stopped  |
| (Measuring)|               | (Idle)    |
+------------+               +-----------+
      |                            |
      | KEY1                       | KEY1
      v                            v
+------------+               +-----------+
| Page 0-3   |               | Page 0-3  |
| Display    |               | Display   |
+------------+               +-----------+
      |                            |
      | Data Ready                 |
      v                            |
+------------+                     |
| Alert/     |                     |
| Feedback   |                     |
+------------+                     |
      |                            |
      | KEY3                       | KEY3
      v                            v
+------------+               +-----------+
| Save to    |               | Save to   |
| Flash      |               | Flash     |
+------------+               +-----------+
      |                            |
      +-------------+--------------+
                    |
                    v
             [ Continue Loop ]
```

## I2C总线共享

```
PB10 (SCL) ----+---- OLED (0x78)
               |
PB11 (SDA) ----+---- MAX30102 (0xAE)
               |
               +---- MPU6050 (0xD0)
               |
               +---- SHT30 (0x88)

注意: 4个设备共享I2C总线，通过不同的I2C地址区分
```

## 中断优先级

```
Priority | Interrupt        | Function
---------|------------------|------------------------
Highest  | SysTick          | 1ms时基，系统调度
         | USART1           | 蓝牙数据接收
Lowest   | (无其他中断)     |
```

## 内存分布

```
Flash (64KB):
  0x08000000 - 0x0800FFFF  | Application Code

RAM (20KB):
  0x20000000 - 0x20004FFF  | Stack, Heap, Variables
  
External Flash (W25Q64 - 8MB):
  0x000000 - 0x7FFFFF      | Health Data Storage
```

## 功能模块关系

```
┌─────────────────────────────────────────┐
│          Health Monitoring System       │
├─────────────────────────────────────────┤
│                                         │
│  ┌──────────────┐    ┌──────────────┐  │
│  │  Vital Signs │    │   Motion &   │  │
│  │  Monitoring  │    │ Environment  │  │
│  │              │    │              │  │
│  │ - MAX30102   │    │ - MPU6050    │  │
│  │ - DS18B20    │    │ - SHT30      │  │
│  └──────────────┘    └──────────────┘  │
│         │                    │          │
│         └─────────┬──────────┘          │
│                   │                     │
│         ┌─────────▼─────────┐           │
│         │  Data Processing  │           │
│         │   & Algorithms    │           │
│         └─────────┬─────────┘           │
│                   │                     │
│         ┌─────────▼─────────┐           │
│         │ User Interface &  │           │
│         │   Feedback        │           │
│         │                   │           │
│         │ - OLED Display    │           │
│         │ - Keys            │           │
│         │ - Buzzer/Motor    │           │
│         └─────────┬─────────┘           │
│                   │                     │
│         ┌─────────▼─────────┐           │
│         │ Data Storage &    │           │
│         │  Transmission     │           │
│         │                   │           │
│         │ - W25Q64 Flash    │           │
│         │ - HC-05 BLE       │           │
│         └───────────────────┘           │
│                                         │
└─────────────────────────────────────────┘
```
