## GD32330C-start
A demo project designed to run on the GigadDvice GD32330C-Start demo board using IAR v7.8.  

### [This project on github](https://github.com/markr1961/GD32330C-start)

### Project notes and documentation
CPU Information, FW and documentation can be found at https://www.gigadevice.com/product/mcu/arm-cortex-m4/gd32f330c8t6 Note: this link changes frequently, so YMMV.
The board documentation is included in the "GD32F3x0 Demo Suites" available at [M4/GD32F3 at GD32MCU](https://www.gd32mcu.com/en/download/7?kw=GD32F3).  
- [direct link to Datasheet (rev 2.6 as of June 2023)](https://www.gigadevice.com.cn/Public/Uploads/uploadfile/files/20230314/GD32F330xxDatasheetRev2.6.pdf)
- [direct link to User Manual (rev 2.7 as of June 2023)](https://www.gigadevice.com.cn/Public/Uploads/uploadfile/files/20230228/GD32F3x0_User_Manual_EN_Rev2.7.pdf)

### Build notes
The project is built using IAR v7.8. It is small enough to be compiled and linked by the Kickstarter Edition.  
There is an add-on for IAR 7 or 8 that lets it work with the GD32F3x0 without having to install a newer version of IAR. [direct link])(https://www.gd32mcu.com/download/down/document_id/219/path_type/1)
Also required is a directory 'GD32F30x_Firmware_Library'. This should be at the same level as the source code.  
This directory must contain the CMSIS and the GD32F3x0 standard peripheral library.  

### Debugger
The board includes a "GD-Link" debugger, which is CMSIS-DAP compatible. The debugger uses the 2 pin SWD GD-link debugger.  
While simple, it seems to work well with IAR.  
There are also flash and programming tools on the [GD32F3 Cortex-M4 family page](https://www.gd32mcu.com/en/download/7?kw=GD32F3)

### Serial COM ports
Unlike demo boards from other vendors, the GD-Link debugger does not include an embedded COM port (at least not that I've found.)
So there is no direct communication option from the target CPU to the system and an external 3.3V USB-serial device is needed.

### CPU
- GD32F330C8T6 Cortex-M4 @ 84MHz max
- 64K flash, 8K RAM
- (2) USART, (2) I2C, (2) SPI
- 1 ADC, 10 channels
- RTC, 7 Timer + Systick
- up to 39 I/O
- 48 pin QFP

### Board notes
- The board has 2 user LEDs and 1 user button.
- LED1:   PF6
- LED2:   PF7
- button: PA0-WKUP
- PF0 and PF1 are connected to an 8MHZ xtal.
- PC14 and PC15 are connected to a 32KHz xtal.
- PA13 and PA14 are used by the SWD debug interface.

SP1 links 3.3V to Vbatt. 
Unless external power is supplied to Vbatt through the right side of SP1, it must be closed by a solder blob to use the RTC.

The board includes a resettable poly fuse on the 5V supply from the USB; which is nice in case you're clumsy like me and accidently short the 3.3V rail.  
The fuse is a SMD1210P005TF and will pass 50mA normally, 150mA max. The 3.3V regulator is a AMS1117-33.  
While the regulator can handle 800mA, the poly fuse is the limiter.  
Both the debug and target CPUs are supplied through the fuse and reg, so don't expect to supply a lot of current to external devices.  

--- running commit notes ---

2023-06-13 MAR
update documentation and links to board/firmware resources.  
