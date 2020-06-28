# DoSC
Diagnostics on Socket CAN

## Aim
The aim of this project is to build a library which makes OBD2 and UDS interface easier

## State of Project

The project is young, running up against a goofy ECU sim so currently working on getting a production ECU on a bench.  Untested against actual vehicles so proceed with caution.  Usage example exists in a local project currently.  Either that or an end user consumable project will be uploaded soon.

#### OBD2:
- Read ECU Names
- Read ECU Faults
- Clear ECU Faults

##### UDS:
Undeveloped beyond ISO-TP support

## Why SocketCan
SocketCan treats the CAN bus as a first class citizen.  USB CAN adapters or native CAN interfaces on SoC provides a more seamless experience than ELM (or similar) interface chips.

While ELM is a fanstastic bit of silicon unifying the legacy OBD protocols all cars since 2008 have been required to implement CAN as one of their interfaces.
This library leans into that fact and ignores non-CAN OBD.  Due to SocketCan's abstraction of hardware any "native" CAN interface may be used with this library.

## Suggested USB Adapters
- PCAN-USB (Peak/Grid Connect)
- CANUSB (lawicel)
- CANtact (cantact.io)

Other adapaters may work these are just suggested

## Protocols

OBD2 - Protocol required by emissions, may also be used for simple datalogging

UDS - More advanced diagnostic protocol, not required but commonly used

## Build

It can easily be built using QT Creator or via cmake in the regular way.

## License

GNU GPL v3

## Contribution

Contribution welcome, please mimic my coding style the best you can.
