# esp8266switch
Firmware for controlling ESP8266 based smart home devices.  A lot of low cost smart home devices are based on the esp8266 chip, like those from Sonoff, KYGNE, KCOOL, KMC, etc.

## Using Platformio Flash Firmware

If you are unfamiliar with ESP8266 development with Platformio for ESP8266 development, find a good tutorial online to follow. 

Before compiling you must checkout my wifimanager project and create a symbolic link to the wifimanager/lib/wifimanager directory in the lib directory of this project.

```
ln -s ../../wifimanager/lib/wifimanager/ wifimanager
```

Check configuration.h file to see if your device is supported and uncomment the define or add the needed pin configuration.


## Using Firmware

Once you have flashed the device, the device will go into AP mode which you can connect to with you computer.  Looks for `switch-[chipid] WiFi SSID.

To configure the device for your network use the following curl comment and query parameters. 

```
curl -X PUT "http://192.168.1.1/config"
```

| Query Parameter  | Decriptions |
| --- | --- |
| ssid | wifi name |
| password | wifi password |
| location | The location of the device, default house |
| room | The room the device is in, default room |
| device | The name of the device, default light |
| mqttHost | Hostname of the mqtt broker |
| disableLed | Disable the LED when device is on |

Once you run the command the device will restart and connect to your network.

## Finding the device ###
The device is discoverable on you network with mDNS via the room and device name.

```
ping office-desk-lamp
```

## REST Controlling the device

Current status

```
curl "http://[IP.ADDRESS]/"
```

Turn light on

```
curl "http://[IP.ADDRESS]/relay/closed"
```

Turn light off

```
curl "http://[IP.ADDRESS]/relay/open"
```

Turn toggle light

```
curl "http://[IP.ADDRESS]/toggle"
```

## MQTT

The device publish to the status topic and listen for commands on the command topic. The topics names are based on the name and location you configured:

Command Topic:
house/[LOCATION]/[NAME]/command

The commands message to publish to control the device are simple numeric commands: 

* 0 - Turn Off
* 1 - Turn On
* 2 - Toggle 
* 3 - Status

Status Topic:
house/[LOCATION]/[NAME]/status


Example using mosquitto client to get the current status

```
mosquitto_pub -t house/office/desk-lamp/command -m '3'
```

Example of mosquitto to listen for published statuses.

```
mosquitto_sub -t house/outside/desk-lamp/status
```
