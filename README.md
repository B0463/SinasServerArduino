# Sinas Server Arduino

**Sinas Server Arduino** is the microcontroller (MCU) side of the [Sinas Server System](https://github.com/B0463/SinasServerSystem). The MCU is responsible for controlling the hardware of a home server, such as fans, signal LEDs, external devices, sensors, and more. In the future, the [Sinas Server System](https://github.com/B0463/SinasServerSystem) will be fully integrated with Sinas Server Arduino.

## Protocol

Communication between the [Arduino](#arduino) and the [controller](#controller) uses the UART protocol. The Arduino connects to the server via a UART-to-USB interface (e.g., FTDI or CH series), which appears as a TTY device on the server.

The protocol is simple and byte-oriented. It consists of:

- 1 byte for the package length
- Followed by the package data

The transmission speed is set to **57600 baud**.

## Arduino

On the MCU side, an Arduino runs a loop that controls a single fan for now. The main loop waits to receive a byte from the [controller](#controller), then begins receiving the rest of the package and executes the corresponding command.

The Arduino can:

- Control fan PWM
- Read sensor values
- Return data to the controller

## Controller

On the controller side, a CLI program sends data to a specified TTY port. It sends a command, waits for a response, displays it, and then closes the connection.

## Usage

1. Upload the `SinasServerArduino.ino` sketch to your Arduino.
2. Connect a power supply to the Arduino.
3. Connect the Arduino's USB interface to your server's motherboard.

**Important note:**  
You may encounter a ground loop between the USB GND and the power supply GND. To avoid issues, it is recommended to use an external FTDI module and optocouplers between the TX and RX lines of the UART.

To use the controller:

1. Build the controller by running the `controller/build.sh` script.
2. Then execute the resulting `controller/dist/ArdController` binary.

Use `dmesg` to find the TTY port assigned to your Arduino, and ensure your user is part of the `dialout` group to access the port.
