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
int wrong_time = 0;
int timer_10_sec = 0;

void check_password();
void unlock_state();
void button_start();
void timer_start();
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
//        UART_Write('\n');
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

void __interrupt(high_priority) Hi_ISR(void)
{
    if(INT0IF == 1)
    {
        INT0IF = 0;
        INT0IE = 0;
        TMR0IF = 0;
//        TMR0IE = 0;
        TMR0ON = 0;
        LED = 0;
        UART_Write_Text("lock again\n");
        return;
    }
    else if(TMR0IF == 1)
    {
        TMR0IF = 0;
        ++timer_10_sec;
        char c = timer_10_sec + '0';
        if(c == 10 + '0')
        {
            UART_Write_Text("10\n");
        }
        else{
        UART_Write(c);
        UART_Write(' ');
        }
        if(timer_10_sec == 10)
        {
            timer_10_sec = 0;
            INT0IF = 0;
            INT0IE = 0;
//            TMR0IE = 0;
            TMR0ON = 0;
            LED = 0;
            UART_Write_Text("lock again\n");
        }
    } 
    
    // solenoid valve lock
    return;
}

void __interrupt(low_priority)  Lo_ISR(void)
{
    if(RCIF)
    {
//        UART_Write_Text("test");
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
//        UART_Write_Text(mystring);
//        UART_Write_Text("test");
        RCIF = 0;
    }
//    UART_Write_Text(mystring);
    
    
    
   // process other interrupt sources here, if required
    return;
}

void main()
{
    OSCCON=0x70;  /* use internal oscillator frequency
                                 which is set to 8 MHz */
    
    UART_Initialize();
    INTERRUPT_Initialize();
    
//    char data_in;
    TRISD = 0;  /* set PORT as output port */
    LATD = 0;
    
    __delay_ms(50);
    while(1)
    {
//        data_in = '\0';     // reset
//        data_in = mystring[0];
        if(mystring[0] == 'l' && mystring[1] == 'o' && mystring[2] == 'c' && mystring[3] == 'k')
        {   
            LED = 0;  /* turn ON LED */
            UART_Write_Text("lock state\n");  /* send LED ON status to terminal */
            ClearBuffer();
        }
        else if(mystring[0] == 'u' && mystring[1] == 'n' && mystring[2] == 'l' && mystring[3] == 'o' && mystring[4] == 'c' && mystring[5] == 'k')
        {
            LED = 1;  /* turn OFF LED */
            UART_Write_Text("unlock state\n");  /* send LED OFF status to terminal */
            ClearBuffer();
            check_password();
        }
        else if(mystring[0] != '\0')
        {
            UART_Write_Text("Option lock or unlock\n");  /* send msg to select proper option */
            ClearBuffer();
        }
        __delay_ms(1000);
    }
    
}

void check_password()
{
    // check password when user press unlock
    UART_Write_Text("What is the password:\n");
    while(1)
    {
        if(wrong_time >= 3)
        {
            UART_Write_Text("Wrong over 3 times\n");
            wrong_time = 0;
            
            ClearBuffer();
            return;
        }
        
        if(mystring[0] == '1' && mystring[1] == '2' && mystring[2] == '3' && mystring[3] == '4')
        {
            UART_Write_Text("Success unlock\n");
            LED = 1;
            wrong_time = 0;
            button_start();
            timer_start();
            // solenoid valve
            
            ClearBuffer();
            return;
        }
        else if(mystring[0] != '\0')
        {
            UART_Write_Text("Password wrong\n");
            UART_Write_Text(mystring);
            ++wrong_time;
            ClearBuffer();
        }
        __delay_ms(1000);
    }
}

void button_start()
{
    INT0IE = 1;
    return;
}

void timer_start()
{
    timer_10_sec = 0;
    TMR0H = 0xE1;
    TMR0L = 0x7D;
    TMR0IE = 1;
    TMR0ON = 1;
    return;
}

void INTERRUPT_Initialize()
{
    RCONbits.IPEN = 1;      //enable Interrupt Priority mode
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1;     //disable low priority interrupt
    INTCONbits.PEIE = 1;
    
    // button interrupt init
    TRISBbits.RB0 = 1;
    ADCON1 = 0x0f;
    INTCON2 = 0x00;     // rising edge interrupt
    INTCONbits.INT0IE = 0;
    INTCONbits.INT0IF = 0;
    PORTB = 0x00;
    
    // timer interrupt init
    TMR0IF = 0;
    TMR0IP = 1;     // high priority
    TMR0IE = 1;
    T0CON = 0x04;   // prescaler = 32;
    // 1sec = (counter value)*Tosc*4*prescaler
    // 1sec = 7812 * 1/8M *4 * 32
    // 65525 +1 - 7812 = 0xE17D
    TMR0H = 0xE1;
    TMR0L = 0x7D;
}