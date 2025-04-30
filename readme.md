#### wheelchair bridge 

Simple circuit using I2C digipot to emulate two-axis joystick following
(reasonable common) DB9 pinout.

Typically, a joystick for a wheelchair generates a +/- 1V around a 6V (mid 12V) center voltage. 

Note that the situation when the pico is powered off (and signals go to zero)