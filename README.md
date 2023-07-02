# MegaOTA
The basis of my Almost Smart Home (ASH) project.

Using
* ATmega328P
* nRF24L01+
* AT24C512
* SST25VF512
* and maybe more over time



## sample input for update over UART

:1038000014BE0FB6F894A8958091600088618093EB  
:103810006000109260000FBECDB7DEB76097DEBFCC  
:10382000CDBF40E150E060EF73E0CE010196F8D0EB  
:103830006F818FEF860F803708F071C088858150C7  
:10384000883008F06CC091E230E020E0F9019093FC  
:1038500057004491E2E0F0E0909357008491E4E057  
:10386000F0E090935700E49190E0B0E0A0E0BA2F30  
:10387000A92F982F8827A42B8E2B4E85CF84D12C4F  
:00000006FA

dump memory
:00000007F9

dump eeprom
:00000008F8

dump flash
:00000009F7

dump nRF24L01+
:0000000AF6

clear memory
:0000000BF5

dump MCU signature
:0000000CF4

dump MCU fuse
:0000000DF3

:0000000EF2

:0000000FF1



export $(grep -v '^#' .env | xargs)
cd main
make -C ../boot clean && make -C ../boot
make clean && make && make update
make -C ../updater clean && make -C ../updater
../updater/updater -d /dev/tty.usbserial-1460 -m ./update_main.bin 0x3C00 -w ./update_write.bin 0x7B80
