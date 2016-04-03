# Trine University Senior Design 2016

## GPS-Aware Autonomous Vehicle

My senior design project for Trine University is to construct a vehicle capable
of independently following a path between two buildings on campus.

## Team

- Colinco, Jedi
- Herber, Mitchell
- Payne, Alexander
- Wiggins, Craig

## Software Development

We are using an Atmel AVR Mega2560 CPU, built onto an Arduino Mega2560 R3 board.
The Arduino framework provides a small hardware abstraction layer and common
library functions, and our code runs atop this foundation.

The Arduino framework and our code are written in C/C++. The project is compiled
using `gcc-avr`, and loaded to the board with `avrdude`.

The [Arduino IDE][arduino] is capable of performing these tasks automatically. I
use the [Platformio][pio] manager as a replacement for the IDE.

Due to the folder structure of the `lib/` tree, the Arduino IDE may not properly
compile and link our libraries into the main executable code under
`SeniorDesign/`. PlatformIO is written in Python2, and will be usable anywhere.

[arduino]: https://arduino.cc
[pio]: https://platformio.org
