# Day 1 – Embedded Systems

## What is an Embedded System?
Embedded software is computer software written to control machines or devices that are not typically considered computers. 
It is specialized for the specific hardware it runs on and usually operates under strict time and memory constraints. 
This term is sometimes used interchangeably with "firmware".

---

## What is a Microcontroller?
A microcontroller (MCU) is a small computer on a single integrated circuit containing:
- CPU
- Built-in RAM and ROM
- Built-in storage
- Built-in ports

Unlike a PC, these components are fixed and not upgradable.

### Typical Internal Components:
- Comparators
- Timers and counters
- Buses
- ADC and DAC
- GPIO, UART, SPI, I2C, PWM, CAN

**Example Output Devices:**
- LM016 LCD / touch screen
- 7-segment displays
- RS-485 / USB to TTL / RS-232 (for debugging or communication)
- Bluetooth, WiFi, LoRa modules

**Example Input Devices:**
- Buttons / keypads
- Communication ports
- Sensors (temperature, humidity, motion, etc.)

---

## Firmware vs Software
**Firmware**: Low-level code that directly controls hardware, ensuring it operates correctly. Typically device-specific and rarely updated.  
**Embedded Software**: Higher-level code that manages system behavior, user interaction, and communication with external systems. More flexible and frequently updated.

| Aspect            | Embedded Firmware                             | Embedded Software                              |
|-------------------|----------------------------------------------|-----------------------------------------------|
| Specificity       | Device-specific, tailored for hardware       | Works across hardware platforms (with FW)     |
| Update Frequency  | Rarely updated                               | Frequently updated                            |
| Primary Focus     | Low-level hardware control                   | Application logic, user interaction           |
| User Interaction  | None                                         | Direct interaction with users or systems      |
| Examples          | ABS in cars, pacemakers                      | Smart thermostat, infotainment systems        |

---

## What is Real-Time?
A real-time system guarantees that it responds within a strict time limit, every time.  

- **Hard real-time**: Missing a deadline is a system failure (e.g., airbags).
- **Soft real-time**: Missing a deadline reduces performance but is not catastrophic (e.g., video streaming).

---

## References
- [Firmware vs Embedded Software Differences – CodeWave](https://codewave.com/insights/firmware-embedded-differences/?utm_source=chatgpt.com)
