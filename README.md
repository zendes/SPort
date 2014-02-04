Arduino library for FrSky SPort
===============================

This library allows your arduino to read FrSky SPort messages, and decode the telemetry info in it.

I used the following sources of info to write it:
* https://code.google.com/p/telemetry-convert/wiki/FrSkySPortProtocol

To connect this to your Arduino, you'll need an inverter cable such as http://www.hobbyking.com/hobbyking/store/__24523__ZYX_S_S_BUS_Connection_Cable.html, or you can build your own using a TTL inverter or the schematic in the included SBUS_inverter.png file.

Basic Installation
------------------
```
$ cd <arduino-sketchbook>/libraries
$ git clone https://github.com/zendes/SPort.git
```

Then restart your Arduino IDE and take a look at the examples.
