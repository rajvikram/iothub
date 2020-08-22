This code is for a smart hub with thermocouple. 


[1] To compile and run the iothub server, download the code to a raspberry pi
	-  


[2] To compile and run the thermocouple code in src/thermocouple one needs the arduino IDE with esp8266 board support.

	- The code runs an an HTTP/HTTPS client on the device and connects to a remote server.
	- Regularly wakes up from deep sleep (ideally) to report the thermocouple value to the serve
