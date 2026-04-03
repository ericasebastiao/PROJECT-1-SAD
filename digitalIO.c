/*
 * File:   digitalIO.c
 * Author: Faculdade
 *
 * Created on 31 de Março de 2026, 12:33
 */


#include "xc.h"
#include "digitalIO.h"

int digital_Read()
{
    return (!PORTDbits.RD6);//Switch case needed
}

void digital_Write(int n)
{
    PORTAbits.RA0 = n;
}
