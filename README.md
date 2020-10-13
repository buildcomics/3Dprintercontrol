# 3D Printer Control
This is the GitHub repository for https://buildcomics.com/XXXX
It contains arduino code for the controller of the arm in the folder named "arduino_motorcontroller".\
There is python code and a service file that controls the Arm, Power and DSLR camera.\
For passing the serial port through a proxy port, use the

## Wiring
A basic connection of raspberry pi pins to the printer smokebox is required. As well as wiring to the smoke detector.
Wiring of the arduino arm (which can be done separately) can be done using any 4 pins connected to the ULN2003A module, power is drawn via the arduino (nano) that comes in from the (powered) USB Hub. Many online good examples available, like this one: https://www.aranacorp.com/en/control-a-stepper-motor-with-arduino/
Schematic for the power control box:

## Installation
IMPORTANT NOTE: These instructions are for my (specific) setup, you might need to change paths, filenames, adresses, ports and more to get it running for your particular setup, depending on the differences.
### 3Dprintercontrol server
You need some python packages installed on your system:
- pyserial
- python gphoto2
- flask
- pigpio
- octorest
Also make sure the following distro packages are installed:
- gphoto2
- libgphoto2-devel (or similar named package)
- pigpio
To run the python code on your "proxy" server, copy the entire folder called "3dprintercontrol"  to /srv/python.\
From there, copy the 3dprintercontrol.service file into your systemd directory of choice (mine was /lib/systemd/system/). This service file is for debian, but might need modification for your distro\
Now run it using $systemctl enable 3dprintercontrol\
Optionally, enable it for auto start: systemctl enable 3dprintercontrol

### Serial port forwarder
Install the ser2net distro package. \
Copy or merge the file ser2net_server/ser2net.conf to /etc/ser2net
Start the serial 2 network service with systemd(systemctl start ser2net)
Optionally, enable it for auto start: systemctl enable ser2net

### Serial port receiver
At the place where you want to use the "remote"  serial port (e.g. the server where octoprint is running), copy in the file ser2net_server/socat.service to your systemd folder. Edit the .service file and put the IP address (or hostname) instead of <IPADDRESS>.
Start and enable the service (see examples above).

### Arduino
Just upload the bloody arduino_motorcontroller code to your arduino of choice...

## License
MIT License

Copyright (c) 2020 buildcomics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
