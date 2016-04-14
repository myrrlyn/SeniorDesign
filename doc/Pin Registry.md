# Pin Registry

This is the master list of which peripheral devices are using which pins on the
Arduino.

## Pins by Device

This lists each device and which pins it occupies.

### GPS

- Enable: 22
- Fix Status: 23
- Tx: 52 OR 18
- Rx: 53 OR 19

### Ultrasonic Sensors

- Dev 00: 24, 25
- Dev 01: 26, 27
- Dev 02: 28, 29
- Dev 03: 30, 31
- Dev 04: 32, 33
- Dev 05: 34, 35
- Dev 06: 38, 39
- Dev 07: 40, 41
- Dev 08: 42, 43
- Dev 09: 44, 45
- Dev 10: 46, 47
- Dev 11: 48, 49

### Motors

- Left Speed: 11
- Left Direction: 8
- Right Speed: 12
- Right Direction: 9

### Switches

- Stop/Run: ~RESET
- Destination Selector: 4

### Tachometer

- Left: 2
- Right: 3

### Compass

- Uses the I2C bus lines above D13.

## Pins by Number

	D00 - Peripheral <- core <- USB
	D01 - Peripheral -> core -> USB
	D02 - Tachometer Left
	D03 - Tachometer Right
	D04 - Destination
	D05
	D06
	D07

	D08 - Motor Left Direction
	D09 - Motor Right Direction
	D10
	D11 - Motor Left Speed
	D12 - Motor Right Speed
	D13

	D14 - TX3
	D15 - RX3
	D16 - TX2
	D17 - RX2
	D18 - TX1 - GPS (Rx)
	D19 - RX1 - GPS (Tx)
	D20 - I2C bus
	D21 - I2C bus

	D22 - GPS (Enable)
	D23 - GPS (Fix)
	D24 - Ultrasonic Sensor
	D25 - Ultrasonic Sensor
	D26 - Ultrasonic Sensor
	D27 - Ultrasonic Sensor
	D28 - Ultrasonic Sensor
	D29 - Ultrasonic Sensor
	D30 - Ultrasonic Sensor
	D31 - Ultrasonic Sensor
	D32 - Ultrasonic Sensor
	D33 - Ultrasonic Sensor
	D34 - Ultrasonic Sensor
	D35 - Ultrasonic Sensor
	D36
	D37
	D38 - Ultrasonic Sensor
	D39 - Ultrasonic Sensor
	D40 - Ultrasonic Sensor
	D41 - Ultrasonic Sensor
	D42 - Ultrasonic Sensor
	D43 - Ultrasonic Sensor
	D44 - Ultrasonic Sensor
	D45 - Ultrasonic Sensor
	D46 - Ultrasonic Sensor
	D47 - Ultrasonic Sensor
	D48 - Ultrasonic Sensor
	D49 - Ultrasonic Sensor
	D50
	D51
	D52 - GPS (Tx)
	D53 - GPS (Rx)

	A00
	A01
	A02
	A03
	A04
	A05
	A06
	A07

	A08
	A09
	A10
	A11
	A12
	A13
	A14
	A15

	IOREF
	~RESET
	VIN
	AREF
