## Flash-bang Detector

Author: Nicanor Pagsanjan, UC Berkeley Space Science Lab

Copyright © 2015. The Regents of the University of California (Regents). All Rights Reserved. Permission to use, copy, modify, and distribute this software and its documentation for educational, research, and not-for-profit purposes, without fee and without a signed licensing agreement, is hereby granted, provided that the above copyright notice, this paragraph and the following two paragraphs appear in all copies, modifications, and distributions. Contact The Office of Technology Licensing, UC Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620, (510) 643-7201, for commercial licensing opportunities.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

#####Device was built using the following libraries:
  + [mBed Standard Library](https://developer.mbed.org/users/mbed_official/code/mbed/)
  + [mBed Real-Time Operating System](https://developer.mbed.org/handbook/RTOS)
  + [EthernetInterface](https://developer.mbed.org/handbook/Ethernet-Interface)
  + [NTPClient](https://developer.mbed.org/cookbook/NTP-Client)
  + [SDFileSystem](https://developer.mbed.org/users/neilt6/code/SDFileSystem/)
  + [MODGPS](https://developer.mbed.org/users/AjK/code/MODGPS/docs/tip/GPS_8h_source.html)
  + [RTC](https://developer.mbed.org/users/Sissors/code/RTC/)


#####SD Card Wiring:


 Pin |  SD  | Description
 --- |------| -----------
  5  |  DI  | SPI mosi
  6  |  DO  | SPI miso
  7  |  SCK | SPI sck
  8  |  CS  | DigitalOut cs
 VCC | VOUT | 3.3V
 GND |  GND | 0V


#####Sample Output Format:

```
Date[ YYYY-MM-DD ] Time[ HH:MM:SS.(microseconds) ]
Type of trigger[L for light, S for sound] Temp[Degrees Celsius]
2015-08-10 12:34:56.789012 L 20.56
```

#####Bugs/ Things to note:
  + SD card doesn't always mount; restarting usually solves the issue
  + Additionally, a quick format on the SD may solve issues
  + Ethernet only works via DHCP
