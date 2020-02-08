# STM32_SNTP
Precise time sync with SNTP for STM32 platform. A project for Embedded Systems class.
## Description
The goal of this project is to synchronize the board's RTC to time obtained from an NTP server. It's basically an implementation of SNTP, plus some blinking LEDs.

## Installation
Just clone the repo and run `make all` in its main directory.  
Set `SNTP_ADDR` and `SNTP_PORT` definitions in Inc/config.h to match desired NTP server.
Make sure your board is connected and recognized by your computer.

## Usage
Connect the programmed board to power and via Ethernet to a DHCP-enabled network with at least one NTP server (preferably one that tolerates spam and is behind as few firewalls as possible, to avoid connection problems).
Upon powering up, LD1 should be turning on and off in one second intervals.
LD3 will indicate connection status:
- CONNECTING (blinking frequently) - LWIP is being initialized, board is waiting for DHCP, etc,
- CONNECTED (glowing steadily) - the LWIP initialization is completed,
- FAILURE (blinking slowly) - LWIP init failed.

If the connection is successful,  the board should now perform an initial synchronization with 1 second of precision.
After that, more precise synchronization can be carried out by pressing USER_Btn1 (the blue one).
That's it. Now LD1 should be turning on and off every second in sync with the server time.

## How it works
LD1 is toggled each second by an RTC Alarm interrupt.  
Ethernet communication is handled by LwIP.  
RTC HAL functions were used to set time. Sub-second sync was achieved with the `SetSynchroShift`, which was a bit tricky.


## How it's made
This project was developed with STM32 CubeMX, ST-Link, OpenOCD and GDB, compiled with GCC and managed via Makefile.

## How it can be improved
- This program assumes UDP packets don't get lost. They do. And if one does get lost, it freezes. **A re-transmission timeout is necessary**.
- External sync. There's none. And the internal RTC of the STM tends to drift a lot more than the datasheet specifies. It **can be calibrated with an external signal**, however we chose not to do it because we wanted to keep dependencies (especially physical ones, like a precise crystal oscillator) to a minimum. We also didn't have a precise crystal oscillator at hand.
- Having multiple boards in the same network will cause glitches, unless they're **configured with different MAC addresses** (in CubeMX or `Inc/ethernetif.h`). Moving it to `config.h` might be a good idea.
- The RTC resolution is not parametrized (enough) and most of the code was written with it assumed to be 255. Moving it to a separate definition in `config.h` and testing is also advised.

## Todo and roadmap and such
Fix issues listed above.
## Authors and acknowledgment
Made by **Mirosław Błażej** (in a much bigger part than commits might suggest, because pair programming) and **Ignacy Grudziński** as a project for Embedded Systems.
