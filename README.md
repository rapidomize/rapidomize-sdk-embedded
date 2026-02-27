# Embedded C/C++ SDK for IoT Edge Devices
Build your own IoT controller solutions using this C/C++ SDK for ESP32 based devices. 
Use it with Rapidomize IoT platform or with any MQTT server.


## H/W Models supported
- Generic ESP32 devices
- [IoT Edge Controller: rpz-d2x2t2ux-we](https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/)


# Features
### System Info
![System Info](/img/home.png)

### Wifi Setup
![Wifi Setup](/img/wifi.png)

### MQTT Setup
![MQTT Setup](/img/mqtt.png)

### Peripheral H/W setup
![Peripherals](/img/peripherals.png)

### Firmware update - OTA & File
![Firmware update](/img/fw.png)

### Local logs
![Firmware update](/img/logs.png)

### Reset
![Factory Reset](/img/reset.png)


## Firmware Installation (Flashing)
Firmware can be first time installed (Flashed) using following tools. Once the firmware is installed into your device, further updates can be installed using the [Firmware update - OTA & File feature](#firmware-update---ota--file). It allows you to install firmware from the release page using the firmware release URL or download a particular release to your local computer and install using the downloaded local firmware binary file.

### First Time installations
Before you start, download the latest release of the [rapidomize-sdk-embedded firmware](https://github.com/rapidomize/rapidomize-sdk-embedded/releases).
Connect your device using a USB cable to your computer. In case of, [IoT Edge Controller: rpz-d2x2t2ux-we](https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/), it also need to be powered up using the 12V power adopter. 

If you are customizing the firmware, download the source release tarball and follow the developer documentation to customize the firmware as you need. 

#### Identify the Serial Port the device is connected to.

- Windows: Either use following command or use Device Manager > **Ports (COM & LPT)**
    ```
    wmic path Win32_SerialPort get DeviceID,Name
    ```

- Linux
    ```
    ls /dev/ttyUSB*
    ```        
    e.g.  /dev/ttyUSB0

- Mac OS
    ```
    ls /dev/tty.*
    ```
    e.g. /dev/tty.usbmodemXXXX



## Flashing with Esptool
[esptool](https://github.com/espressif/esptool) : a Python-based open-source, platform-independent commandline utility for flashing. Steps to install the firmware:
- Download & extract the latest binary release for your operating system & CPU architecture from [esptool release page](https://github.com/espressif/esptool/releases). 
- OR if you have Python3 in your system, install it via `pip`.
```
pip install esptool
```

- Erase Flash
```
esptool --chip esp32 --port PORT erase_flash
```
- Upload/Write Flash
```
esptool -p PORT -b 460800 --chip esp32 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x10000 rapidomize-sdk-embedded-0.7.5.b24.bin
```

where PORT is the USB serial port identified above.

After successfully executing the command, esptool will hard reset the chip, causing it to run the firmware

## Flash Download Tool
If you have a computer with Windows you can use `Flash Download Tool` to flash the firmware.
- Download & extract the latest binary release [Flash Download Tool](https://dl.espressif.com/public/flash_download_tool.zip)

[Flash Download Tool User Guide](https://docs.espressif.com/projects/esp-test-tools/en/latest/esp32/production_stage/tools/flash_download_tool.html)

# Status
version 0.7.5 - 'Dugong Weasel'

# Contributions?
Contributions are highly welcome. If you’re interested in contributing leave a note with your username.

# Policy for Security Disclosures
If you suspect you have uncovered a vulnerability, **contact us privately**, as outlined in our security policy document; we will immediately prioritize your disclosure.


# License
Apache 2.0