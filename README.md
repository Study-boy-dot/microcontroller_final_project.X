# final_project.X
 microcontroller final project

## Device used
1. PIC18F4520
2. bluetooth module HC-05
![image](https://user-images.githubusercontent.com/80616480/149746031-a7a6c527-f846-4bd3-aae9-fd61b7d2a068.png)
![271793667_510540383629781_8235616375555372857_n](https://user-images.githubusercontent.com/80616480/149746132-6a530109-50e3-405f-9804-44376a7af33e.jpg)

## Bluetooth Init
Similar with uart init
baund rate set as 9600Hz
(Fosc = 8Mhz)

```
//  Setting baud rate
TXSTAbits.SYNC = 0;         // asynchronous mode           
BAUDCONbits.BRG16 = 0;          
TXSTAbits.BRGH = 0;
```
Use application below to connect with HC-05
![271655495_467018465042736_315294439475738600_n](https://user-images.githubusercontent.com/80616480/149746215-dc992c6e-b2c6-4d18-9918-410c5177e925.jpg)
