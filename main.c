#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "p24fj1024gb610.h"
#include "UART1.h"
#include "ADC.h"

int main(int argc, char** argv){
    
    int adcvalue = 0;
    int button = 0;
    
    ADC_Setup();
    UART1Config();
    
    ANSD = 1;
    TRISA = 0;
    TRISDbits.TRISD6 = 1;
    
    while(1){
        //adcvalue = ADC_Read(4);
        digital_Write(digital_Read());
        UART1_PrintINT (button); //podemos mudar isto para sprintf()
        UART1_Write('\n');
    }
    
}
































/*
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "p24fj1024gb610.h"

//#define Fos 8000000
//#define PreScalar 256
//#define IPerS Fos / 2 / PreScalar
//#define FCY 4000000
//
//// Configuration Bits TIMER1
//#pragma config FNOSC = PRI
//#pragma config POSCMOD = HS
//#pragma config JTAGEN = OFF
//#pragma config FWDTEN = OFF
//
//void setupTimer1()
//{    
//    TMR1 = 0;
//    PR1 = IPerS;
//    IPC0bits.T1IP = 0x7;
//    IFS0bits.T1IF = 0;
//    IEC0bits.T1IE = 1;
//    // 0bx1000 0000 0011 0000
//    T1CON = 0x8030;
//}
//
//void __attribute__ ( ( interrupt, __shadow__ ) ) _T1Interrupt(void)
//{
//    IFS0bits.T1IF = 0;
//    TMR1 = 0;
//} 

int main(int argc, char** argv) {
    ANSD = 1;
    TRISA = 0;
    TRISDbits.TRISD6 = 1;
    
    //LATA = 0xFFFF;
    LATA = 0;

    
    while(1){
       // run
       if (!PORTDbits.RD6) {
           //LATA = ~LATA;
           PORTAbits.RA0 = 1;
           PORTAbits.RA2 = 1;
       }
       if (!PORTDbits.RD7) {
           //LATA = ~LATA;
           PORTAbits.RA0 = 0;
           //PORTAbits.RA2 = 0;
       }
   }
    return (1);
}
*/