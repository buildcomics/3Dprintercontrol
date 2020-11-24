# 3D Printer Control
This is the GitHub repository for the project: https://buildcomics.com/hero/3dp-a-new-hero-and-its-home/ \
You will also find the "instructions"  there! \
 \
This repo contains arduino code for the controller of the arm in the folder named "arduino_motorcontroller".\
There is python3 code and a service file that controls the Arm, Power and DSLR camera.\
For passing the serial port through a proxy port, use the ser2net_server/client folders containing setup file (examples).
Bash scripts for the integration with octolapse are in the octolapse directory

## Models
Find all the models for the arm and more on \
thingiverse:\
or prusaprinters:

## Wiring
A basic connection of raspberry pi pins to the printer smokebox is required. As well as wiring to the smoke detector.
Wiring of the arduino arm (which can be done separately) can be done using any 4 pins connected to the ULN2003A module, power is drawn via the arduino (nano) that comes in from the (powered) USB Hub. Many online good examples available, like this one: https://www.aranacorp.com/en/control-a-stepper-motor-with-arduino/
Schematic for the power control box: TODO

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

Now rename or copy the config.EXAMPLE.py to config.py, and add the data for your setup.
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
Install the socat distro package if needed. \
At the place where you want to use the "remote"  serial port (e.g. the server where octoprint is running), copy in the file ser2net_server/socat.service to your systemd folder. Edit the .service file and put the IP address (or hostname) instead of <IPADDRESS>.
Start and enable the service (see examples above). You should now be able to connect to the serial port you forwarded.

### Arduino
Just upload the bloody arduino_motorcontroller code to your arduino of choice...

### Octolapse
Copy the scripts to a directory on your octoprint/octolapse server. \
In the octolapse settings, add the prints start_arm.sh/stop_arm.sh to your camera start/stop print scripts for the camera that is mounted on the arm.\
The same goes for the DSLR control if you use it, but use start_snapshot.sh/stop_snapshot.sh scripts. Also add snapshot_acquire.sh to the take snapshot capture script. \
More info about setting up octolapse using scripts: https://github.com/FormerLurker/Octolapse/wiki/V0.4---Configuring-a-DSLR

### Cameras
There are two service files to run mjpg_streamer as a systemd daemon. \
mjpg_streamer.service is for the Logitech C910. \
mjpg_streamer2.service is for the Logitech C310.\
1. Copy the files to your systemd directory (for me that is /etc/system/systemd/mjpg_streamer.service).
2. Enable the service using systemctl start mjpg_streamer


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
