#include "UART1.h"

void UART1Config (){
    __builtin_write_OSCCONL(OSCCON & 0xbf); //serve para fazer unlock PPS
    RPOR8bits.RP17R=0x0003; //UART1 - > U1TX no RF5
    RPINR18bits.U1RXR = 0x000A;
    
    U1BRG = 25; //Com base nos 9600 bits/s (baud rate) para uma Fosc = 8 MHz
    
    U1MODE = 0x8000; //Caso não funcione, um mode 0x8884 é uma excelente 2ª hipótese
    U1STA = 0x0400; //status register for what? we don't know, afinal sabemos é a página 258
}

void UART1_Write (char data) {
    while (U1STAbits.UTXBF); //possivelmente pode sair daqui :) mas U1STAbits.UTXBF = 1 <=> buffer cheio
    
    U1TXREG = data; //buffer de transmissão
}

char UART1_Read (void) {
    if (U1STAbits.URXDA) {
        return U1RXREG;
    }
    
    return '\0';
}

void UART1_Print (char *str)
{
    int i = 0;
    while (str [i]!= '\0'){
        UART1_Write (str [i]);
        i++;
    }
}


void UART1_PrintINT (int n)
{
    if (n < 10)
        UART1_Write(n + 48);
    else
    {
        UART1_PrintINT(n /10);
        UART1_Write((n %10)+48);
        
    }
}


int UART1_Available (void) {
    if (U1STAbits.URXDA) {
        return 1;
    }
    return 0;
}
