/*
 * File:   main.c
 * Author: junee
 *
 * Created on January 17, 2022, 3:22 PM
 */


#include <pic18f4520.h>
#include <xc.h>
//#include "usart.h"
#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ 1000000
#define LED LATD0    

char mystring[20];
int lenStr = 0;

void INTERRUPT_Initialize();

void UART_Initialize() {
           
        //    TODObasic   
        //    Serial Setting      
        // 1.   Setting Baud rate
        // 2.   choose sync/async mode 
        // 3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        // 3.5  enable Tx, Rx Interrupt(optional)
        // 4.   Enable Tx & RX
    
    // UART Initialize baund rate = 9600hz
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;       // Input            
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;         // asynchronous mode           
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 0;
    SPBRG = 12;      // lookup table fosc = 8Mhz
    
   //   Serial enable
    RCSTAbits.SPEN = 1;         // Asynchronous serial port
    PIR1bits.TXIF = 0;          // interrupt flag
    PIR1bits.RCIF = 0;          // will set when reception complete
    TXSTAbits.TXEN = 1;         // enables transmission
    RCSTAbits.CREN = 1;         // continuous receive data
//    PIE1bits.TXIE = 1;          // enable transmit interrupt
//    IPR1bits.TXIP = 1;          // enable transmit interrupt priority             
    PIE1bits.RCIE = 1;          // enable receive interrupt              
    IPR1bits.RCIP = 0;          // enable receive interrupt priority  
}

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}

char *GetString(){ // 
    return mystring;
}

void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 20 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead()
{
    /* TODObasic: try to use UART_Write to finish this function */
    unsigned char c = RCREG;
    
    if(c == '\r'){
        UART_Write('\n');
//        ClearBuffer();
//        UART_Write(c);
        return;
    }
    mystring[lenStr] = c;
    ++lenStr;
//    UART_Write_Text(mystring);
//    UART_Write(c);
    return ;
}

void strcpy_in_uart(char *str)
{
    int i =0;
    for(i = 0; i < lenStr ; i++){
        str[i] = mystring[i];
    }
    str[i] = '\0';
}

void __interrupt(low_priority)  Lo_ISR(void)
{
    if(RCIF)
    {
        UART_Write_Text("test");
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
        UART_Write_Text(mystring);
//        UART_Write_Text("test");
        RCIF = 0;
    }
//    UART_Write_Text(mystring);
    
    
    
   // process other interrupt sources here, if required
    return;
}

void main()
{
    OSCCON=0x72;  /* use internal oscillator frequency
                                 which is set to 8 MHz */
    
    UART_Initialize();
    INTERRUPT_Initialize();
    
    char data_in;
    TRISD = 0;  /* set PORT as output port */
    LATD = 0;
    
    __delay_ms(50);
    while(1)
    {
        data_in = '\0';     // reset
        data_in = mystring[0];
        if(data_in=='1')
        {   
            LED = 0;  /* turn ON LED */
            UART_Write_Text("LED OFF");  /* send LED ON status to terminal */
        }
        else if(data_in=='2')
        {
            LED = 1;  /* turn OFF LED */
            UART_Write_Text("LED ON");  /* send LED OFF status to terminal */
        }
//        else
//        {
//            UART_Write_Text("Something wrong");  /* send msg to select proper option */
//        }
        __delay_ms(1000);
    }
    
}

void INTERRUPT_Initialize()
{
    RCONbits.IPEN = 1;      //enable Interrupt Priority mode
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1;     //disable low priority interrupt
    INTCONbits.PEIE = 1;
}