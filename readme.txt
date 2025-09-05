In rccarmain.c, it has an interrupt handler and a main loop. In the handler, it is taking in two ADC values from
the joystick on the remote controller, one for x, one for y, and then transmitting that on UART. In main, it is 
receiving the ADC value through UART and translating the values the motors corresponding speed using two PWM
function, one for each motor. UART transmission is blind-cycle while UART receiving is interrupt based.
There are a lot of functions at the beginning of the code which were used in different methods of getting 
the car to run, and not all of them are in use.

