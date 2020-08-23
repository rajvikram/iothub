### This repo implements a smart hub on a Raspberry Pi Zero with a thermocouple sensor on a ESP8266 board. 


[1] To compile and run the iothub server, download the code to a raspberry pi
	-  


[2] To compile and run the thermocouple code in src/thermocouple one needs the arduino IDE with esp8266 board support.

	- Regularly wakes up from deep sleep (ideally) to report the thermocouple value to the server


### Raspberry pi zero OS headless setup:
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


