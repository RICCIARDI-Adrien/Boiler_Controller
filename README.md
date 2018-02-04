# Boiler Controller

## Hardware
Schematics have been drawn using Eagle 8.3.1.  

### Power board
This board contains the AC/DC converter, the relays and the connector to the wall housing.
  
![Power board schematics](https://github.com/RICCIARDI-Adrien/Boiler_Controller/blob/master/Hardware/Power_Board_Schematics.png)
  
![Power board PCB](https://github.com/RICCIARDI-Adrien/Boiler_Controller/blob/master/Hardware/Power_Board_PCB.png)

### Controller board
This board holds the microcontroller and the WiFi to UART bridge.
  
![Controller board schematics](https://github.com/RICCIARDI-Adrien/Boiler_Controller/blob/master/Hardware/Controller_Board_Schematics.png)

![Controller board PCB](https://github.com/RICCIARDI-Adrien/Boiler_Controller/blob/master/Hardware/Controller_Board_PCB.png)

### Components
* [Single pole 16A relay with 5V coil](http://fr.farnell.com/oeg-te-connectivity/omih-ss-105lm-300/relais-spst-no-250vac-30vdc-16a/dp/1891727)
* [BD135 NPN bipolar transistor](http://fr.farnell.com/stmicroelectronics/bd135/transistor-npn-to-126/dp/9801529)
* [1N4007 rectifier diode](http://fr.farnell.com/multicomp/1n4007/diode-1a-1000v/dp/9565051)
* [15pF ceramic capacitor](http://fr.farnell.com/multicomp/mc0805n150j101a2-54mm/condensateur-mlcc-c0g-np0-15pf/dp/2112750)
* [100nF ceramic capacitor](http://fr.farnell.com/multicomp/mc0805b104k500a5-08mm/condensateur-mlcc-0-1uf-50v-x7r/dp/2395773)
* [10uF electrolytic capacitor](http://fr.farnell.com/multicomp/mcgpr16v106m5x11/condensateur-elec-alu-10uf-16v/dp/9451056)
* [10uF tantalum capacitor](http://fr.farnell.com/multicomp/mctar16v106kc/condensateur-tantale-10uf-16v/dp/2395815)
* [5V 2A AC/DC power supply](http://fr.farnell.com/vigortronix/vtx-214-010-205/alimentation-ac-dc-5v-2a/dp/2464685)
* [160ohm 1% resistor](http://fr.farnell.com/multicomp/mf12-160r/resistance-0-125w-1-160r/dp/9342672)
* [330ohm 5% resistor](http://fr.farnell.com/multicomp/mcf-0-25w-330r/resistance-couche-carbon-330r/dp/9339418)
* [470ohm 1% resistor](http://fr.farnell.com/multicomp/mf50-470r/resistance-0-5w-1-470r/dp/9340610)
* [1Kohm 5% resistor](http://fr.farnell.com/multicomp/mcf-0-25w-1k/resistance-couche-carbon-1k-0/dp/9339051)
* [1Kohm 1% resistor](http://fr.farnell.com/multicomp/mf50-1k/resistance-1k-0-5w-1/dp/9339779)
* [Fuseholder](http://fr.farnell.com/littelfuse/64900001039/porte-fusible-cms-20x5mm/dp/1271673)
* [Fuse](http://fr.farnell.com/littelfuse/0232002-mxp/fuse-cartridge-medium-acting-2a/dp/1707348)
* [2-pad terminal](http://fr.farnell.com/camdenboss/ctb0708-2/bornier-standard-2-voies-30-12awg/dp/2315273)
* [4-pad terminal](http://fr.farnell.com/camdenboss/ctb0708-4/bornier-standard-4-voies-30-12awg/dp/2315275)
* [Atmega328p microcontroller](http://fr.farnell.com/microchip/atmega328p-pu/micro-8-bits-avr-32k-flash-28pdip/dp/1715487)
* [3.6864MHz crystal](http://fr.farnell.com/iqd-frequency-products/lfxtal003263/quartz-3-686400mhz/dp/9712909)
* [3.3V LDO](http://fr.farnell.com/texas-instruments/lm1086ct-3-3-nopb/ic-reg-ldo-3-3v-1-5a-to-220/dp/1564628)
* [WiFi to UART bridge](https://www.gotronic.fr/art-module-wifi-uart-grove-113020010-23827.htm)
* [4-pin PCB male connector](https://www.gotronic.fr/art-connecteur-s004-5858.htm)
* [4-pin PCB female connector](https://www.gotronic.fr/art-connecteur-p004-5867.htm)
* [10-pin PCB male connector](https://www.gotronic.fr/art-connecteur-s010-15367.htm)
* [10-pin PCB female connector](https://www.gotronic.fr/art-connecteur-p010-15370.htm)

### Analog signals characteristics
* Day trimmer : resistance variation from 50 to 110ohm.
* Night trimmer : resistance variation from 0 to 60ohm.
* Radiator water start thermistor : resistance variation from 580 (+80°C) to 900ohm (-20°C).
* Outside thermistor : resistance variation from 340 (+40°C) to 480ohm (-10°C).
  
Microcontroller uses a 3.3V precision voltage reference for the ADC module to gain more precision with small variation signals. All voltage dividers must be calculated as the highest variable resistance value results in a 3.3V maximum voltage on the ADC pin.

### Fuse value calculation
AC/DC converter inrush current is 40A for less than 500us, so a temporized fuse is needed to allow the board to boot.  
AC/DC converter maximum input current is 150mA @ 240V, but due to enormous inrush current a big fuse must be used. A 250V/2A fuse has been chosen because this is the fuse used on the VTX-214 demo board (http://www.farnell.com/datasheets/1878969.pdf).

## Software

### Building
Install `avr-gcc` to build the microcontroller firmware.
  
Go to `Software/Microcontroller_Firmware` directory and type the following command to build the firmware :
```
make
```
File `Boiler_Controller_Firmware.elf` will be created.

### Flashing
You need to install `avrdude` to access to the programmer.  
Firmware can be burnt to the microcontroller memory by connecting an AVR ISP programmer to the controller board ISP connector and typing the command :
```
make flash
```
  
You can override the `PROGRAMMER_SERIAL_PORT` environment variable with the serial port your programmer is connected to.
