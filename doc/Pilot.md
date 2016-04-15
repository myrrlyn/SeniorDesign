# Pilot Module

This module governs the actual behavior of the motors. It responds to certain
abstract commands (go forward, go backward, turn left, turn right) and sets the
motors as appropriate.

The motors are driven using PWM waves to control their speed. A 0% duty cycle
halts the motor entirely; a 100% duty cycle lets the motor run continuously;
duty cycles in the interim cause rapid start/stop cycles.

The relays we are using have an operating frequency of 15kHz. The motors will
see the lower PWM of the chip vs the relay. Higher is better, so they don't
scream. The relays have some back-flow protection to deal with the fact that
DC motors in rapid toggle are really hard on electronics.

## Software

We are using pins 11 and 12 on the Arduino, which are mapped to PB5 and PB6 on
the AVR. PB5 and PB6, in addition to being GPIO, are also mapped to OC1A and
OC1B, respectively, and can be driven independently of the CPU by the Timer 1.

### Timer 1

We will be setting up Timer 1 to deliver fast PWM, non-inverting waves, on pins
11 and 12.

Fast PWM &ndash; counts from `0x0000` to whatever our `TOP` variable is. When
the counter reaches `OCR1A`, `OCR1B`, or `OCR1C`, the corresponding `OC1x` pin
falls from `1` to `0`. In this case, we will only be using A and B; C must be
disabled for ordinary operation of its pin.

The COM mode for this is `0b10`. The WGM mode is `0b1110`.

#### Register Values

###### `TCCR1A`

This register contains { `COM1A[1:0]`, `COM1B[1:0]`, `COM1C[1:0]`, `WGM1[3:2]` }

- `COM1x`: `0b10`. In Fast PWM, this causes OC1x to be HIGH while the count is
less than `OCR1x` and LOW while the count is greater than `OCR1x`.
- `WGM1[3:2]`: `0b11`. This sets the high two bits of Wave Generation Mode. We
want WGM 14 (binary pattern `1110`).

We will set it to the value `0b10100011`, `0xA3`.

###### `TCCR1B`

This register contains { `ICNC1`, `ICES1`, `-`, `WGM1[1:0]`, `CS1[2:0]` }

- `ICNC1`: `0b0`. We do not want noise filtering on `ICP1`.
- `ICES1`: `0b0`. We are not using edge detection on `ICP1`.
- `-`: This bit is reserved and must be set to `0`.
- `WGM1[1:0]`: `0b10`. This sets the low to bits of Wave Generation Mode. We are
using WGM 14 (binary pattern `1110`).
- `CS1[2:0]`: `0b001`. This is the clock prescaler. It can divide the main clock
by 1, 8, 64, 256, or 1024. We want a prescaler value of 1.

TODO: Look at calculating prescaler values.

We will set it to the value `0b00010001`, `0x11`.

###### `TCCR1C`

This register contains { `FOC1A`, `FOC1B`, `FOC1C`, `-----` }

The `FOC1x` bits force output compare. We will be disabling them.

The value of this register is `0b00000000`, `0x00`.

###### `TCNT1H` and `TCNT1L`

These are the High and Low bytes of the counter. Do. Not. Write. To. These. We
also don't care about reading from them.

As with all 16-bit registers in high and low pairs, write High, Low and read
Low, High.

###### `OCR1xH` and `OCR1xL`

These are the Output Compare Registers for the A, B, and C markers. When the
counter is equal to one of these registers, the action on the corresponding pin
is taken. In this case, `OC1x` falls from HIGH to LOW (fast PWM).

We will be using `OCR1A` and `OCR1B`. These registers contain the 16-bit number
determining the duty cycle for the corresponding motor.

Duty cycle is the register value divided by the TOP value. WGM 14 uses the value
in `ICR1` as TOP, so, duty cycle is `OCR1x` / `ICR1`.

Note that shrinking `ICR1` makes the PWM output *even faster*, and we don't
really care about precision all that much. When the counter hits `ICR1`, it is
reset.

The values of these registers are set by the Pilot software.

###### `ICR1`

This is the maxium value to which the counter will increment. When the counter
reaches this value, it is reset to 0.

The PWM waveform is HIGH as the counter runs from 0 to OCR, and LOW from OCR to
this value. The duty cycle percentage is therefore the ratio between OCR and
ICR.

###### `TIMSK1`

This is the interrupt mask. We are not using this timer for any software
interrupts, so the mask will be fully disabled.

###### `TIFR1`

When an interrupt goes off, the matching flag in this register is also set. If
we cared, we would write a logic 1 to the flag bit to clear it, but we don't.

###### `DDRB`

AVR architecture uses each pin for two functions: general- and special- purpose
IO. We need to set the pins used by `OC1A` and `OC1B` to explicit output in
order for the clocks to drive them, and we must take care to not use those pins
directly in software. `OC1A` shares Pin 11 with `PB5`, and `OC1B` shares Pin 12
with `PB6`. The `DDRB` register must have bits 5 and 6 set high; the rest should
be left alone. The Arduino function `pinMode()` does this, but for consistency's
sake I am using bit-level initialization for this as well as for the rest of the
timer registers.
