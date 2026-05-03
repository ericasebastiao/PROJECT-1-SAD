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
int digital6_array[MAX_BUFFER_PORT];
int digital7 = 0;
int digital7_array[MAX_BUFFER_PORT];
int bidirecional = 0;
int bidirecional_array[MAX_BUFFER_PORT];
int virtual = 0;
int virtual_array[MAX_BUFFER_PORT];
int state_bidirecional = 0;
int state_virtual = 0;
volatile int time_sampling = 1; //segundos
volatile int samples = 5; //Numero de amostras a enviar de cada vez
int digitalwrite1 = 0; //Saida digital
int digitalwrite2 = 0; //Saida digital
int digitalwrite3 = 0; //Saida digital

//FLAGS, Interrupt decide enviar e 
volatile int read = 0;
volatile int i_samples = 0;


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
    position = strstr(json, "\"DW1\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digitalwrite1 = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"DW2\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digitalwrite2 = atoi(position + 1); 
        }
    }
    position = strstr(json, "\"DW3\""); 
    if (position != NULL) { 
        position = strchr(position, ':'); 
        if (position != NULL) {
            digitalwrite3 = atoi(position + 1); 
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
    if (digital6 && !state_virtual) {
        send_array_json("D6", digital6_array, samples, &first);
    }
    if (digital7 && !state_virtual) {
        send_array_json("D7", digital7_array, samples, &first);
    }
    if (bidirecional && state_bidirecional) {
        send_array_json("DB", bidirecional_array, samples, &first);
    }
    if (virtual && state_virtual) {
        send_array_json("DV", virtual_array, samples, &first);
    }
    UART1_Print ("}\r\n");

}


int main(int argc, char** argv){
    timer_initialize();
    ADC_Setup();
    UART1Config();
    
    ANSD = 1;
    TRISA = 0;
    ANSAbits.ANSA7 = 0;
    TRISDbits.TRISD6 = 1;
    TRISDbits.TRISD7 = 1;
    
    
    //AS 3 linhas seguintes inicializam os 3 pinos de saída como saída
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    

 
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
        if (bidirecional){
        if (!state_bidirecional) 
        {
            TRISAbits.TRISA7 = 0;  
            LATAbits.LATA7 = 1;    
        }
        else 
        {
            LATAbits.LATA7 = 0;   
            TRISAbits.TRISA7 = 1;  
        }}

            LATAbits.LATA0 = digitalwrite1; 
            LATAbits.LATA1 = digitalwrite2;
            LATAbits.LATA2 = digitalwrite3;
        if (read)
        {
            if (i_samples < samples)
            {
                if (analogPortX)
                    analogPortX_array[i_samples] = ADC_Read(4);
                if (analogPortY)
                    analogPortY_array[i_samples] = ADC_Read(5);
                if (analogPortZ)
                    analogPortZ_array[i_samples] = ADC_Read(2);
                if (digital6 && !state_virtual)
                    digital6_array[i_samples] = digital_Read(54);
                if (digital7 && !state_virtual)
                    digital7_array[i_samples] = digital_Read(55);
                if (bidirecional && state_bidirecional)
                {
                    TRISAbits.TRISA7 = 1;
                    bidirecional_array[i_samples] = digital_Read(56);
                }    
                if (virtual && state_virtual)
                    virtual_array[i_samples] = 2*digital_Read(55) + digital_Read(54);
                
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

    return 0;
}