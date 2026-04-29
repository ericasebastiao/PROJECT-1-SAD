#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>

#include "p24fj1024gb610.h"
#include "UART1.h"
#include "ADC.h"
#include <libpic30.h> 

// 2. Defini��es de Frequ�ncia para Delays
#define FOSC    8000000UL        // 8 MHz
#define FCY     (FOSC / 2)       // 4 MHz (Ciclo de Instru��o)
#define MAX_JSON_SIZE = 1024;
#define MAX_SAMPLES = 10;
#define LCD_RS  LATBbits.LATB15
#define LCD_E   LATDbits.LATD4
#define LCD_RW  LATDbits.LATD5
#define LCD_DATA LATE

// 1. Bits de Configura��o (Essenciais para evitar que o PIC reinicie)
#pragma config FWDTEN = OFF      // Watchdog Timer desligado
#pragma config FNOSC = PRI       // Oscilador Prim�rio (8MHz)
#pragma config POSCMD = XT       // Cristal de Quartzo (XT)
#pragma config ICS = PGD1        // Debugging nos pinos standard

int variavelRandom = 0;
int freqControl = 3; // 4*T(em segundos) - 1

// INTERRUPT FLAGS
int PrintFLAG = 0;
int SampleFLAG = 0;

void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
    if (variavelRandom < freqControl){
        variavelRandom++;
        
        IFS0bits.T1IF = 0;
        return;
    }
    
    //FLAGS ONLY
    PrintFLAG = 1;
    
    variavelRandom = 0;

    /* Interrupt Service Routine code goes here */
    IFS0bits.T1IF = 0; //Reset Timer1 interrupt flag and Return from ISR   
}

//JSON example {"OUTPUT1":1, "" } - 
/*void JsonConstructer (char* JSON, int outputAnalog0[], int outputAnalog1[],int outputAnalog2[],int outputdigital0[],int outputdigital1[], int outputdigitalB[]){
     //Clean the json
    int i = 0;
    for(i = 0; i<MAX_JSON_SIZE ; i++){
        JSON[i] = '\0';
    }
    
    sprintf(JSON, "{\"A0\":[");
    
    
    char A0_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){A0_samples[i] = '\0';}
    
    char temp[10];
    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        
        sprintf(temp, "%d", outputAnalog0[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(A0_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(A0_samples, "]");
    strcat(JSON, A0_samples);
    
    
    //////////////////////////////////////////////////////
    
    
    strcat(JSON, ",\"A1\":[");
    char A1_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){A1_samples[i] = '\0';}


    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        
        sprintf(temp, "%d", outputAnalog1[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(A1_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(A1_samples, "]");
    strcat(JSON, A1_samples);
    
    
    //////////////////////////////////////////////////////
    
    
    strcat(JSON, ",\"A2\":[");
    char A2_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){A2_samples[i] = '\0';}


    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        
        sprintf(temp, "%d", outputAnalog2[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(A2_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(A2_samples, "]");
    strcat(JSON, A2_samples);
    
    
    //////////////////////////////////////////////////////
    
    
    strcat(JSON, ",\"D0\":[");
    char D0_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){D0_samples[i] = '\0';}


    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        
        sprintf(temp, "%d", outputdigital0[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(D0_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(D0_samples, "]");
    strcat(JSON, D0_samples);
    
    
    //////////////////////////////////////////////////////
    
    
    strcat(JSON, ",\"D1\":[");
    char D1_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){D1_samples[i] = '\0';}


    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        sprintf(temp, "%d", outputdigital1[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(D1_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(D1_samples, "]");
    strcat(JSON, D1_samples);
    
    
    
    
    strcat(JSON, ",\"DB\":[");
    char DB_samples[MAX_JSON_SIZE];
    for(int i=0; i<MAX_JSON_SIZE; i++){DB_samples[i] = '\0';}


    for(int i=0; i<SamplesN; i++){
        //CLEAN
        for(int i=0; i<10; i++){temp[i] = '\0';}
        
        sprintf(temp, "%d", outputdigitalB[i]);
        
        if(i<(SamplesN-1)){strcat(temp, ",");}
        
        
        strcat(DB_samples, temp);
        //printf("\n%s", A0_samples);
    }
    strcat(DB_samples, "]");
    strcat(JSON, DB_samples);
    
    strcat(JSON, "}");
    
    
    printf("%s", JSON);

    return 0;
}*/

void JsonParser(){
    ;
}

void LCD_Pulse(void) {
    LCD_E = 1;
    __delay_us(50); // Aguarda estabiliza��o
    LCD_E = 0;
    __delay_us(50);
}

void LCD_Command(unsigned char cmd) {
    LCD_RS = 0; // Modo Comando
    
    // Envia os 4 bits superiores
    LATE = (LATE & 0xFFF0) | ((cmd >> 4) & 0x0F);
    LCD_Pulse();
    
    // Envia os 4 bits inferiores
    LATE = (LATE & 0xFFF0) | (cmd & 0x0F);
    LCD_Pulse();
}

void LCD_Char(char data) {
    LCD_RS = 1; // RS = 1 para Dados
    
    // Envia nibble superior
    LATE = (LATE & 0xFFF0) | ((data >> 4) & 0x0F);
    LCD_Pulse();
    
    // Envia nibble inferior
    LATE = (LATE & 0xFFF0) | (data & 0x0F);
    LCD_Pulse();
    
    __delay_us(100);
}

// Envia uma string completa
void LCD_String(const char* str) {
    while(*str) {
        LCD_Char(*str++);
    }
}

void LCD_Init(void) {
    __delay_ms(100);    // Aguarda o LCD estabilizar ap�s power-on
    
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 0;

    // Sequ�ncia de Reset para modo 4-bit
    LATE = (LATE & 0xFFF0) | 0x03; LCD_Pulse(); __delay_ms(5);
    LATE = (LATE & 0xFFF0) | 0x03; LCD_Pulse(); __delay_us(200);
    LATE = (LATE & 0xFFF0) | 0x03; LCD_Pulse();
    
    LATE = (LATE & 0xFFF0) | 0x02; // Configura modo 4-bit definitivo
    LCD_Pulse();

    LCD_Command(0x28);  // 2 linhas, matriz 5x7
    LCD_Command(0x0C);  // Display ON, Cursor OFF
    LCD_Command(0x06);  // Incremento autom�tico do cursor
    LCD_Command(0x01);  // Limpa ecr�
    __delay_ms(5);
}


int main(int argc, char** argv){
    
    int adcvalue = 0;
    int button = 0;

    T1CON = 0x00; //Stops the Timer1 and reset control reg.
    TMR1 = 0x00; //Clear contents of the timer register
    PR1 = 0xFFFF; //Load the Period register with the value 0xFFFF
    IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
    // (This example assigns level 1 priority)
    T1CONbits.TCKPS1 = 0; //define prescaler 1:8
    T1CONbits.TCKPS0 = 1;
    IFS0bits.T1IF = 0; //Clear the Timer1 interrupt status flag
    IEC0bits.T1IE = 1; //Enable Timer1 interrupts
    T1CONbits.TON = 1; //Start Timer1 with prescaler settings at 1:1 and
    //clock source set to the internal instruction cycle
    
    ADC_Setup();
    UART1Config();
    
    ANSD = 1;
    TRISA = 0;
    TRISDbits.TRISD6 = 1;

     int analogPort1 = 4;
    int analogPort2 = 5;
    int analogPort3 = 6;

    //char Config_Json[MAX_JSON_SIZE] = ""; 
    //char Sender_JSON[MAX_JSON_SIZE] = ""; 
    
    //LCD
    
    //TRISA = 0x0000; 
    //TRISE = 0x0000;
    TRISE &= 0xFFF0; 
    TRISDbits.TRISD4 = 0; 
    TRISDbits.TRISD5 = 0;
    TRISBbits.TRISB15 = 0; 
    
    ANSELBbits.ANSB15 = 0; // Garante que RB15 � Digital (importante!)
    
    LCD_Init();

    /*    LCD_RW = 0; 
    
 
    __delay_ms(50);     
    LCD_Command(0x02);  
    LCD_Command(0x28);  
    LCD_Command(0x0C);   
    LCD_Command(0x01);  
    __delay_ms(2);
 */
      
      
      

    // 3. Escrever a Mensagem
    LCD_Command(0x80);    
    LCD_String("START");
    
    //LCD_Command(0xC0);        
    //LCD_String("PIC24FJ1024");
    
    while(1){
        
    }

    /*while(1){
        //adcvalue = ADC_Read(4);
        digital_Write(digital_Read());
        UART1_PrintINT (button); //podemos mudar isto para sprintf()
        UART1_Write('\n');
    }*/

     /*while(1){
        int RD6_value = digital_Read(54); //Pin 54 -> RD6
        int RD7_value = digital_Read(55); //Pin 55 -> RD7
        
        
        int adc1 = ADC_Read(analogPort1);
        int adc2 = ADC_Read(analogPort2);
        int adc3 = ADC_Read(analogPort3);
        
        //config
        //NAO ACABADO PRECISA
        if (UART1_Available()) {
            char received = UART1_Read();
   
        }
        UART1_Write('a');
        if(PrintFLAG){
            UART1_Write('o');
            UART1_Write('\n');
            PrintFLAG = 0;
        }
    }*/
    
    
    return 0;
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
