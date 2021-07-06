### This repo implements a smart hub on a Raspberry Pi Zero with a thermocouple sensor on a ESP8266 board. 


#### [1] The devices are supposed to connect to an influx/Grafana hub. This can be run on a Raspberry pi which can be configured as follows

Raspberry pi zero OS headless setup:
- Download the Lite image. Flash it to the SD card using Raspberry Pi Imager
- Remove card and re-insert to mount it on the host OS. There should be 2 partitions `boot` and `rootfs`
- Create / touch a file called `ssh` in the rootfs root dir
- Create a file called `wpa_supplicant.conf` in the rootfs root dir with the following:
```
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
ssid="xxxx"
scan_ssid=1
psk="123456"
key_mgmt=WPA-PSK
}

```
- Change hostname in `etc/hostname` to `iothub`
- Change the default password on the `pi` user
- Setup password-less login by copying the host public key in `home/pi/.ssh/authorized_keys`
- Turn off the power management on the wifi by backing up the `etc/network/interfaces` and then copying the following in there:
```
auto lo

iface lo inet loopback
iface eth0 inet dhcp

auto wlan0
allow-hotplug wlan0
iface wlan0 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
wireless-power off
iface default inet dhcp
``` 
Confirm power mangement is off in the `iwconfig` listing
- For stable ssh connection, add the following to `etc/ssh/sshd_config`. Some home routes apparently have a QoS issue with ssh packets. 
```
IPQoS cs0 cs0
```
- Eject SD card from host and boot Pi Zero with it. You should be able to `ssh pi@iothub.local` and get a responsive console

#### Install Influx and grafana on Raspberry Pi.

- For Influx follow these instructions: https://pimylifeup.com/raspberry-pi-influxdb/ 
	- Check influx is up by running `influx` cli client on Pi zero. You should also be able to connect to the 8086 port from an external machine.
	- Create the `iot` database as `create database iot`
	- If data is published from the device, it will be under the `temperature` table and you can check this by doing a `SELECT * FROM temperature`

- For Grafana follow instructions for v8.0.0 for *Debian ARMv6* : https://grafana.com/grafana/download/8.0.0?platform=arm
	- Once grafana is installed you should be able to access it external at http://iothub.local:3000
	- Create dashboard with InfluxDB as datasource and essentially create a query which says `SELECT hot,cold FROM temperature WHERE device='solar_oven' OR import the dashboard JSON file in this repo.

#### [2] To compile and run the thermocouple code in src/thermocouple one needs the arduino IDE with esp8266 board support.
- Configure the Arduino IDE to support the ESP8266 board and install the following dep libraries
	- Adafruit MAX31856 
	- Influxdb client (v3.x)
- Configure your Wifi SSID and password as compiler flags that get picked up during compile. They need to be in specified a file called *platform.local.txt* with the following line:
	`compiler.cpp.extra_flags=-DDEVSSID="*****" -DDEVPSK="********"`
  The location of the file seems to change depending on the version of arduino IDE but it needs to be in the same directory as the *platform.txt* file. The two known locations are:
	- ~/snap/arduino/current/.arduino15/packages/esp8266/hardware/esp8266/2.7.3/
	- ~/snap/arduino/current/.arduino15/packages/esp8266/hardware/esp8266/2.7.4/
- Configure the iothub server address correctly. 
- Compile and run the code on the board.


#### Notes:
- Once ESP.deepSleep() is used in the code, we need to have GPIO16 and the RST pin connected as shown [here.](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/) .*However* you can NOT flash code if the pins are connected. So the pins should be connected AFTER the code is flashed.

#### ESP8266 with MAX31856

![Circuit Diagram](thermocouple_diag.png?raw=true)
 
