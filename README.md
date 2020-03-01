# relay-arduino
Relay Arduino for communicating in between sensors and desktop.

Libraries to install in Arduino IDE:
- CAN by Sandeep Mistry
- Ethernet and SD libraries are already provided by default.

## UDPCANRelay + CANEcho
Input: `stdbuf -o0 nc -u 192.168.0.9 2323`
Output: `stdbuf -o0 nc -lu 192.168.0.6 2323`

### Note on SPI
SPI CS pin conflicts are possible when you're using multiple SPI libraries (i.e. CAN, Ethernet, SD). The code in UDPCANRelay is written to not keep any SPI connection open, which hopefully causes the library to disable the corresponding CS pin.

- i.e. if you keep TCP connection alive the library might be keeping the CS pin active. But not sure how this works exactly.
