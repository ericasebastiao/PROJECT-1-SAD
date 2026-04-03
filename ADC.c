/*
 * File:   ADC.c
 * Author: Faculdade
 *
 * Created on March 24, 2026, 12:14 PM
 */

#include "xc.h"
#include "ADC.h"

void ADC_Setup () {
    AD1CON1bits.ADON = 1;
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.FORM0 = 0;
    AD1CON1bits.FORM1 = 0;
    AD1CON1bits.ASAM = 0;  
    
}

int ADC_Read (int channel) {    
    AD1CHS = channel;  //0b avisa o compilador que o numero esta em binario
    AD1CON1bits.SAMP = 1;
    
    //esperar
    for(int i=0; i<1000; i++){;}
   
    AD1CON1bits.SAMP = 0;
    while(AD1CON1bits.DONE == 0){}
    return ADC1BUF0;
}

/* int ADC_Available () {
    if (AD1CON1.DONE) {
        return 1;
    }
    
    return 0;
} */




