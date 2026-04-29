/*
 * File:   timer.c
 * Author: Faculdade
 *
 * Created on 7 de Abril de 2026, 11:25
 */

/* The following code example will enable Timer1 interrupts, load the Timer1
Period register and start Timer1.
When a Timer1 period match interrupt occurs, the interrupt service
routine must clear the Timer1 interrupt status flag in software.
*/

#include "xc.h"
#include "timer.h"

T1CON = 0x00; //Stops the Timer1 and reset control reg.
TMR1 = 0x00; //Clear contents of the timer register
PR1 = 0xFFFF; //Load the Period register with the value 0xFFFF
IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
// (This example assigns level 1 priority)
IFS0bits.T1IF = 0; //Clear the Timer1 interrupt status flag
IEC0bits.T1IE = 1; //Enable Timer1 interrupts
T1CONbits.TON = 1; //Start Timer1 with prescaler settings at 1:1 and
//clock source set to the internal instruction cycle
/* Example code for Timer1 ISR*/
void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
/* Interrupt Service Routine code goes here */
IFS0bits.T1IF = 0; //Reset Timer1 interrupt flag and Return from ISR
}

#include "xc.h"

int main(void) {
    return 0;
}

