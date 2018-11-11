# ninmote

Project that puts a remote control into a snes gamepad.

I started this project with a attiny4313 but switched to a ``atmega328p`` because it is very cheap and has also pico power capabilities.

I think you can easily replace it with a atmega48p/atmega88p or atmega168p if you want.

The existing code makes use of the power down functionality of the AVR uC. It only wakes up if a button is pressed (pin change irq) and will go back to sleep after a timeout or sending a signal.

The tx pin is ``PD5 (OC0B)``, the example input pin is ``PB1``

## State

The code is fully working but supports until now only one button (``PB1``).

### Code
- [x] panasonic transmit code
- [x] power down/wakeup logic
- [x] debounce keys

### Schematic/PCB
- [x] misssing

## Setup
The code is made for a ``atmega328p`` running at ``1MHz``.
I use an external 8MHz crystal and enabled the ``DIV8`` flag.
### Fuses
```
avrdude -U lfuse:w:0x7f:m -U hfuse:w:0xd1:m -U efuse:w:0xff:m
```
