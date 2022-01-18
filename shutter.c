#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ 10000000

unsigned char pwm = 16;
unsigned char state = 0;        // open or close

void main(void)
{
    // Interrupt from button setup
    ADCON1bits.PCFG = 0b1111;
    TRISBbits.RB0 = 0b1;    // port b 0's bit as input (button)
    PORTB = 0;
    INTCON2 = 0;
    RCONbits.IPEN = 0b1;
    INTCONbits.INT0IF = 0b0;
    INTCONbits.INT0IE = 0b1;
    INTCONbits.GIE = 0b1;
    
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 ?s
    OSCCONbits.IRCF = 0b001;
    OSCCONbits.SCS = 0b11;  // internal oscillator

    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8?s * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x04*4 + 0) * 8?s * 4
     * = 0.512 ~= 500?s 
     */
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b00;
    
    TRISDbits.RD2 = 1;
    while(1);
    //{
    //    pwm = 16;
    //    CCP1CONbits.DC1B = pwm & 0x03;
    //    CCPR1L = pwm >> 2;
    //}
    
    return;
}

void __interrupt() isr(void)
{
    INTCONbits.INT0IF = 0;  // clear flag bit
    
//    INTCONbits.GIE = 0b0;

//    while(1){
////         -90 to 90
//        if(pwm < 75){
//            ++pwm;
//        }
//        else{
//            pwm = 16;
//            CCP1CONbits.DC1B = pwm & 0x03;
//            CCPR1L = pwm >> 2;
//            break;
//        }
//        CCP1CONbits.DC1B = pwm & 0x03;
//        CCPR1L = pwm >> 2;
//        _delay(500);
//    }
    if(state == 0)
    {
        pwm = 46;
        CCP1CONbits.DC1B = pwm & 0x03;
        CCPR1L = pwm >> 2; 
        state = 1;
    }
    else if(state == 1)
    {
        pwm = 16;
        CCP1CONbits.DC1B = pwm & 0x03;
        CCPR1L = pwm >> 2; 
        state = 0;      
    }
//     __delay_ms(5);
    
 
    return;
}