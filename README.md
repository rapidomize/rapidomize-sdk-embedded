# Embedded C/C++ SDK for IoT Edge Devices
Build your own IoT controller solutions using this C/C++ SDK for ESP32 based devices. 
Use it with Rapidomize IoT platform or with any MQTT server.


## Models supported
- Generic ESP32 devices
- [IoT Edge Controller: rpz-d2x2t2ux-we](https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/)


# Features
- System Info
![System Info](/img/home.png)

- Wifi Setup
![Wifi Setup](/img/wifi.png)

- MQTT Setup
![MQTT Setup](/img/mqtt.png)

- Peripheral H/W setup
![Peripherals](/img/peripherals.png)

- Firmware update - OTA & File
![Firmware update](/img/fw.png)

- Local logs
![Firmware update](/img/logs.png)

- Reset
![Factory Reset](/img/reset.png)


# First Time Installing (Flashing)
Firmware can be first time installed (Flashed) using following tools. Once the firmware is installed, futher updates can be installed using the [Firmware update - OTA & File feature](#first-time-installing-flashing-firmware)


Before you start, download the latest release of the [rapidomize-sdk-embedded firmware](https://github.com/rapidomize/rapidomize-sdk-embedded/releases)

If you are customizing the firmware for your own h/w, download the source release tarball and follow the developer documentation to customize the firmware as you need. Otherwise, you can use the biary release available here for the IoT Gateway you use.

## Esptool
[esptool](https://github.com/espressif/esptool) : a Python-based open-source, platform-independent commandline utility for flashing. Steps to install the firmware:
- Download & extract the latest binary release for your operating system & CPU architecture from [esptool release page](https://github.com/espressif/esptool/releases)
- 

After successfully executing the command, esptool will hard reset the chip, causing it to run the firmware

# Status
version 0.7.5 - 'Dugong Weasel'

# Contributions?
Contributions are highly welcome. If you’re interested in contributing leave a note with your username.

# Policy for Security Disclosures
If you suspect you have uncovered a vulnerability, **contact us privately**, as outlined in our security policy document; we will immediately prioritize your disclosure.


# License
Apache 2.0