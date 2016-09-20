#Garden Irrigation Control
This project was created as a fun learning experience for me with PICs (arduino dumilanove) and to solve tiny itch I had about my garden needing to set the analog clock it had to control irrigation times.

I migh or migh not update this project in the future with new features, and I will be creating a side project to control this system from a phone with or without a server web interface to gather the data, that's yet to be seen.

Of course this system  could have many other uses besides irrigation control. Basically any timed electrical switching could be handled with this.
Hacking this code to use the sensor data to perform actions based on the sensor output shoulnd't be too hard. I might do that in the future, or maybe adding an earth humidity sensor and performing actions inresponse to that, i might do that some day.

###Features:
Ritht now the hardware consists on an arduino, ethernet shield + micro SD card reader, an RTC module that happens to have both, RTC and EEPROM ICs and an humidity sensor that has built-in thermometer.

- It logs the temperature and humidity to an sd file.
- Loads the network configuration from a file called net.cfg on the root of the micro SD card.
- Controls two relays with 4 on-off alarms consisting on an start and end time (HH:MM:SS).
- Exposes a serie of HTTP APIs to the connected network that allows log download, relay control, date and time configuration and instant sensor and status information access.

#Hardware
- Arduino UNO or compatible board
- HanRun Ethernet shield with Micro SD card reader
- I2C RTC module (Tiny RTC) obtained from [here](http://www.dx.com/p/tiny-rtc-real-time-clock-module-for-arduino-red-161172) which consists on:
-- DS1307 RTC chip
-- Atmel AT24C32 EEPROM 32K chip
- Kyees funduino ver: 2R1B relay module from [here](http://www.dx.com/p/arduino-2-channel-relay-shield-module-red-144140)

###Hardware setup:
The ethernet module is simply attached to the arduino board on the standard way.
As some of the pins are used by the ethernet shield, I used the next ones to connect the relay inputs to the arduino digital pins 8 and 9 outputs (IN1 -> digital 9, IN2 - digital 8).
As for the DHT11 (humidity sensor) I used the pin 7.
The I2C RTC clock SDA goes to Arduino analog pin 4 and the SCL goes to arduino analog pin 5 ([if you decide to use another arduino board you should change according to the arduino documentation](https://www.arduino.cc/en/Reference/Wire))

#Configuring network:
To configure the network on the device, you must place a file like the provded net.cfg.example file called net.cfg on the root directory of your sd card.
You must follow the next rules:

	MAC ADDRESS  that will use the device
    IP ADDRESS that will have the device on your network
    NETMASK of the network
    GATEWAY of the network
    DNS SERVER of the network
    
This is an example of a working NET.CFG file:

	DE:AD:BE:EF:FE:ED
	192.168.4.6
	255.255.255.0
	192.168.4.1
	192.168.4.1

#HTTP API
Last, but not least, the HTTP API:
All the date and time API endpoints will use the following format due to hardware limitations and code simplicity:
> YEAR/MONTH/DAY/DAY_OF_WEEK/HOUR/MINUTE/SECOND

- Date
	- **setTime** Configures the RTC clock with the given date and time.
    > 	http://ip_address/setTime/YEAR/MONTH/DAY/DAY_OF_WEEK/HOUR/MINUTE/SECOND

	> Example:
	> http://192.168.4.6/setTime/2016/09/20/2/17/29/30 would set the time and date to tuesday 20th of september of the year 2016 and the time to 17:29:30

	- **getTimes** Prints a plain text dump of the current date and time and the configured alarms.

- Alarms:
	- There is a total of 8 defined alarms, 4 for the each relay. Each alarm consists  on two times in between which the relay will be active, so it's important to **set them both!** There is no specific api to disable an alarm, but if you need to the procedure it's simple: set the start and end time to the same and it shouldn't fire.
	- All the alarm configurations are saved on the EEPROM and persisted in case of power failure.
	- **setAlarm** Configures an alarm.
		- This endpoint has three parameters, the alarm number, the operation type (start/stop) and the time.
		- The date part specified on this api will be ignored, but it's requiered.
	- >		http://ip_address/setAlarm/ALARM_NUMBER(1-8)/ALARM_MODE(start/stop)/DATE_AND_TIME_URL_FORMATTED
	 > Example:
	 > http://192.168.4.6/setAlarm/1/start/2016/09/20/2/17/29/30 **AND another call to** http://192.168.4.6/setAlarm/1/start/2016/09/20/2/19/00/00 would configure an alarm to start everyday at 17:19:30 and stop at 19:00:00

- Direct relay control
	- To control the relays two endpoints are available (sw1 and sw2), both have the same functionality: manually enabling, dissabling and controlling the relays.
	- **sw*X*** Access switch ***X***, replace X with 1 or 2.
		- **on** Turn the relay on.
		- **off** Turn the relay off.
			- **force** Force the relay to stay in off state even if an alarm would fire. This will be saved even after device restart and until the next call to on or off endpoints is made.
    - > Example:
     http://192.168.4.6/sw1/on Would turn the first relay on
     http://192.168.4.6/sw1/off Would turn the first relay off and let alarms fire
     http://192.168.4.6/sw1/off/force Would rutn the first relay off and prevent alarms from turning it on.

- Logs
	- To gather the logs that the system has generated a simple call to /log is needed. Note that the data log will be deleted once the transfer has finished.
	- **log** Get the logs and clear the file
	- The log format is a line containing each measurement, each line is confirmed of the current date and time and the sensors data
	- Example output:
	- 	> <2016-09-20 20:20:10>H:20;T:35;
		> <2016-09-20 20:21:10>H:21;T:34;
	- >		http://ip_address/log

	> Example:
	> http://192.168.4.6/log

- Sensors
	- **getSensors** Prints a simple line containing the sensors information and relay status. The format of the returned text is: sensor:status;other_sensor:status;...
		- **H** is an integer representing the relative humidity
		- **T** is an integer representing the temperature
		- **SW1** and **SW2** is an integer representing the boolean status of the relay 1 and 2

#Case
I made a simple 3D case, but the cap won't stick to the body unless you use tape, if anybody is willing to make a better design it's welcome, just make a merge request.

This case will work with the provided hardware specs, but I can't warrant that it will with another kind of module brands that might be perfectly compatible with this code as I designed it to be quiet a tight fit.
The case has space for a prototyping board and an extra board on top of this, so you can make a pretty circuit and integrate all the components.

I made the case so it's rain resistant, but that will depend on your 3D printer, material and printer configuration. Check the results that come from your printer!