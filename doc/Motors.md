# Motors

Physical control of the GPS Robot is achieved by means of two DC motors
controlled by the Arduino. The motors in question are 24V wheelchair motors
arranged in side-by-side differential steering. They are powered by two 12V
automotive batteries linked in series, and are in parallel to each other. Each
motor has independent control from the Arduino. A 60V 50A control board
serves to interface the power and control circuits and prevent disaster^[1].

Each motor is thus able to accelerate between full stop and full speed, in one
direction only, to steer the vehicle.

## Batteries

The batteries are linked in series. There is an emergency switch on the link
between them, and when thrown will break the entire power circuit open. This
link sees the full current draw of the system. Our estimations place this at
80 to 100 amps, so the switch must be able to take it. Our switch is rated for
100A.

## Control Boards

The control boards connect to both the positive and negative terminals of the
batteries. There is no direct connection between the batteries and motors. As we
used DC motors, the positive and negative motor terminals are not a strict
electrical requirement but rather simply govern the direction the motors will
turn under power. The control boards we used do not have the capacity for
bidirectional control. We were hoping to use two boards in parallel, and switch
between them, but this proved EXTREMELY problematic and was rapidly undone. As
it stands, the motors can only spin forward.

There are two logic headers on the control boards. The two-pin header is simply
an on-off switch. Shorting the pins together turns the board on; opening them
turns it off. This header must never, ever touch the Arduino; it rests at supply
voltage (for us, 48V). It does not need to be programmatically controlled; a
mechanical failsafe switch will serve perfectly well to enable and disable the
control boards. The three-pin header receives analog control signal on the
center pin, ground reference on the pin towards the two-pin header, and bleed
voltage on the pin towards the transistor bank. The boards are designed and ship
with a variable-resistor controller, so the third pin was used to complete the
circuit. The Arduino does not need to deliver to it, and it should be left
floating.

## Arduino

Deliver PWM to the signal pin. Faster is better, as the boards are designed for
analog, not PWM, input. PWM causes the boards to slave to that signal, and that
makes the motors scream. We did not have an electrical engineer so we did not
feel comfortable setting up an RC element to smooth the Arduino output to make
the board perform as nicely under our control as it did under the variable
resistor. (The math was fine; the actual assembly was something we did not want
to risk.)

## Behavior

The motors are extremely powerful for their size. Startup should be done with a
controlled accelerator function to bring them steadily up from 0 to cruising
speed. We found that the motors had to run at full speed to satisfy our load and
route requirements, but even at half and below they performed very well.

We estimate current draw per motor under load at anywhere between 30 and 50 A.
We have not been able to measure directly.

On jacks, it takes 3A to spin an axle and wheel. Applying the brake increases
the draw to 10A without noticeably affecting motor performance. Pulling 150lb of
cargo, in addition to the robot's own weight, popped our 20A fuses immediately.
We have had 50A fuses on each motor since then, without issue.

## Power Consumption

Under 150lb cargo load, we are able to get one or two full trips out of the
motors before the batteries were too drained to continue. It is worth noting
that the motors stop spinning around the 8-12V mark, so the batteries are never
completely emptied. It is **imperative** that, when the robot winds down from
power drain, the circuit is broken and it is brought to recharge under external
power. As voltage dwindles, the motors will compensate by trying to pull more
and more current in order to supply the physical power needed to move. This will
pop the fuses and damage the control boards.

**Do not continue to run the robot after it has slowed or stopped on its own.**

[1]: This did not work absolutely perfectly, as the control boards did not come
with a manual or datasheet. During experimentation, we blew up an Arduino and
one USB port on the attached laptop.
