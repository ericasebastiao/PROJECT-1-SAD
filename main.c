#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>

#include "p24fj1024gb610.h"
#include "UART1.h"
#include "ADC.h"
#include "timer.h"
#include <libpic30.h> 

// 2. Definiï¿½ï¿½es de Frequï¿½ncia para Delays
#define FOSC    8000000UL        // 8 MHz
#define FCY     (FOSC / 2)       // 4 MHz (Ciclo de Instruï¿½ï¿½o)
#define MAX_JSON_SIZE = 1024;
#define MAX_SAMPLES = 10;
#define LCD_RS  LATBbits.LATB15
#define LCD_E   LATDbits.LATD4
#define LCD_RW  LATDbits.LATD5
#define LCD_DATA LATE

#define MAX_BUFFER_PORT 20

// 1. Bits de Configuraï¿½ï¿½o (Essenciais para evitar que o PIC reinicie)
#pragma config FWDTEN = OFF      // Watchdog Timer desligado
#pragma config FNOSC = PRI       // Oscilador Primï¿½rio (8MHz)
#pragma config POSCMD = XT       // Cristal de Quartzo (XT)
#pragma config ICS = PGD1        // Debugging nos pinos standard

int variavelRandom = 0;
int freqControl = 1; // 4*T(em segundos) - 1

// INTERRUPT FLAGS
int PrintFLAG = 0;
int SampleFLAG = 0;

//Buffer json, indice, e flag json completo 
char json[1024];
int i_json = 0;
int json_complete = 0;

//Variaveis do JSON
int analogPortX = 0;
int analogPortX_array[MAX_BUFFER_PORT];
int analogPortY = 0;
int analogPortY_array[MAX_BUFFER_PORT];
int analogPortZ = 0;
int analogPortZ_array[MAX_BUFFER_PORT];
int digital6 = 0;
int digital7 = 0;
int bidirecional = 0;
int virtual = 0;
int state_bidirecional = 0;
int state_virtual = 0;
volatile int time_sampling = 1; //segundos
volatile int samples = 5; //Numero de amostras a enviar de cada vez
int digital1write = 0; //Saida digital
//FLAGS, Interrupt decide enviar e 
volatile int read = 0;
volatile int i_samples = 0;

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

void timer_initialize(){
    T1CON = 0x00; //Stops the Timer1 and reset control reg.
    TMR1 = 0x00; //Clear contents of the timer register
    PR1 = 0xF423; // tinhamos assim anteriormente 0xFFFF; //Load the Period register with the value 0xFFFF
    IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
    // (This example assigns level 1 priority)
    T1CONbits.TCKPS1 = 1; //define prescaler 1:64
    T1CONbits.TCKPS0 = 0;
    IFS0bits.T1IF = 0; //Clear the Timer1 interrupt status flag
    IEC0bits.T1IE = 1; //Enable Timer1 interrupts
    T1CONbits.TON = 1; //Start Timer1 with prescaler settings at 1:1 and
//clock source set to the internal instruction cycle
}

void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
  
    variavelRandom++; 

    
    if (variavelRandom >= time_sampling) 
    {
        read = 1;               
        PrintFLAG = 1;         
        variavelRandom = 0;     
    }
    IFS0bits.T1IF = 0; 
}

void JsonParser(){
    char *position;

    position = strstr(json, "\"b\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            state_bidirecional = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"v\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            state_virtual = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"Ax\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            analogPortX = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"Ay\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            analogPortY = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"Az\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            analogPortZ = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"D6\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digital6 = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"D7\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digital7 = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"DB\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            bidirecional = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"DV\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            virtual = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"p\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            time_sampling = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"n\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            samples = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"DW\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digital1write = atoi(position + 1); 
        }
    }
}
void send_array_json(char* port, int* array, int num_samples, int* first)
{
    char buffer[16];

    //Condicao para saber se põe virgula
    if (!(*first)) {
        UART1_Print (",");
    }

    UART1_Print ("\"");
    UART1_Print (port);
    UART1_Print ("\":[");

    // Imprime os valores do array separados por vírgula
    for (int i = 0; i < num_samples; i++) {
        sprintf(buffer, "%d", array[i]);
        UART1_Print (buffer);
        if (i < (num_samples - 1)) {
            UART1_Print (",");
        }
    }
    UART1_Print ("]");

    *first = 0;
    
}
void send_json()
{
    int first = 1; 

    UART1_Print ("{");

    if (analogPortX) {
        send_array_json("Ax", analogPortX_array, samples, &first);
    }
    if (analogPortY) {
        send_array_json("Ay", analogPortY_array, samples, &first);
    }
    if (analogPortZ) {
        send_array_json("Az", analogPortZ_array, samples, &first);
    }

    UART1_Print ("}\r\n");

}


int main(int argc, char** argv){
    
    int adcvalue = 0;
    int button = 0;

    timer_initialize();
    ADC_Setup();
    UART1Config();
    
    ANSD = 1;
    TRISA = 0;
    TRISDbits.TRISD6 = 1;

    //char Config_Json[MAX_JSON_SIZE] = ""; 
    //char Sender_JSON[MAX_JSON_SIZE] = ""; 
    
    //LCD
    
    //TRISA = 0x0000; 
    //TRISE = 0x0000;
    TRISE &= 0xFFF0; 
    TRISDbits.TRISD4 = 0; 
    TRISDbits.TRISD5 = 0;
    TRISBbits.TRISB15 = 0; 
    
    ANSELBbits.ANSB15 = 0; // Garante que RB15 ï¿½ Digital (importante!)
    
 
    while(1){
        if (UART1_Available())
        {
            char received = UART1_Read();
            json[i_json] = received;
            i_json++;
            if (received == '}')
            {
                json_complete = 1;
                json[i_json] = '\0';
            }
                
            if (i_json >= 1023)
                i_json = 0;   
        }
        if (json_complete)
        {
            JsonParser();
            json_complete = 0;
            i_json = 0;
            i_samples = 0;
        }
        if (read)
        {
            if (i_samples < samples)
            {
                if (analogPortX)
                    analogPortX_array[i_samples] = ADC_Read(4);
                if (analogPortY)
                    analogPortY_array[i_samples] = ADC_Read(5);
                if (analogPortZ)
                    analogPortZ_array[i_samples] = ADC_Read(6);
                //CONTINUAR ISTO PARA TODAS AS PORTAS
                i_samples++;
                if (i_samples >= samples)
                {
                    send_json();
                    i_samples = 0; 
                }
            }
            read = 0;
        }
        
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
