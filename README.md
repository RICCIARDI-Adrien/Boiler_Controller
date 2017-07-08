# Boiler Controller

## Hardware
Schematics have been drawn using Eagle 7.5.  
  
![Main board schematics](https://github.com/RICCIARDI-Adrien/Boiler_Controller/blob/master/Hardware/Main_Board_Schematics.png)

## Components
* [Single pole 16A relay with 5V coil](http://fr.farnell.com/oeg-te-connectivity/omih-ss-105lm-300/relais-spst-no-250vac-30vdc-16a/dp/1891727)
* [BD135 NPN bipolar transistor](http://fr.farnell.com/stmicroelectronics/bd135/transistor-npn-to-126/dp/9801529)
* [1N4007 rectifier diode](http://fr.farnell.com/multicomp/1n4007/diode-1a-1000v/dp/9565051)
* [15pF ceramic capacitor](http://fr.farnell.com/multicomp/mc0805n150j101a2-54mm/condensateur-mlcc-c0g-np0-15pf/dp/2112750)
* [100nF ceramic capacitor](http://fr.farnell.com/multicomp/mc0805b104k500a5-08mm/condensateur-mlcc-0-1uf-50v-x7r/dp/2395773)
* [470nF ceramic capacitor](http://fr.farnell.com/kemet/c330c474k5r5ta/condensateur-mlcc-x7r-470nf-50v/dp/1457703)
* [5V 2A AC/DC power supply](http://fr.farnell.com/xp-power/ecl10us05-e/psu-10w-5v-encaps-pcb-mount/dp/1435356)
* [91ohm resistor](http://fr.farnell.com/multicomp/mf12-91r/resistance-0-125w-1-91r/dp/9343652)
* [160ohm resistor](http://fr.farnell.com/multicomp/mf12-160r/resistance-0-125w-1-160r/dp/9342672)
* [750ohm resistor](http://fr.farnell.com/multicomp/mf12-750r/resistance-0-125w-1-750r/dp/9343512)
* [1Kohm resistor](http://fr.farnell.com/multicomp/mcf-0-25w-1k/resistance-couche-carbon-1k-0/dp/9339051)
* [1.3Kohm resistor](http://fr.farnell.com/multicomp/mf12-1k3/resistance-0-125w-1-1k3/dp/9342575)
* [Fuseholder](http://www.farnell.com/datasheets/2020897.pdf)
* [Terminal](http://fr.farnell.com/camdenboss/ctb0708-4/bornier-standard-4-voies-30-12awg/dp/2315275)
* [Atmega328p microcontroller](http://fr.farnell.com/microchip/atmega328p-pu/micro-8-bits-avr-32k-flash-28pdip/dp/1715487)
* [2.048V precision voltage reference](http://fr.farnell.com/texas-instruments/ref3020aidbzt/ref-de-tension-series-2-048v-sot/dp/1470323)
* [3.6864MHz crystal](http://fr.farnell.com/iqd-frequency-products/lfxtal003263/quartz-3-686400mhz/dp/9712909)

## Analog signals characteristics
* Day trimmer : resistance variation from 50 to 110ohm.
* Night trimmer : resistance variation from 0 to 60ohm.
* Internal thermistor : resistance variation from 580 (+80°C) to 900ohm (-20°C).
* External thermistor : resistance variation from 340 (+40°C) to 480ohm (-10°C).
  
Microcontroller uses a 2V precision voltage reference for the ADC module to gain more precision with small variation signals. All voltage dividers must be calculated as the highest variable resistance value results in a 2V maximum voltage on the ADC pin.
