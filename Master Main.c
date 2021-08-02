//******************************************************************************
/* 
 * File:   Comunicación SPI.c
 * Author: Daniel
 *
 * Created on July 29, 2021, 11:34 AM
 */
//******************************************************************************

//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "SPI header.h"
#include "USART header.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

unsigned char VOLTAJE[10] =  {'0','1','2','3','4','5','6','7','8','9',};

char temp_ADC1;
char temp_ADC2;
char estado;
int Low_ADC1;
int Low_ADC2;
int High_ADC1;
int High_ADC2;

int contadorNum = 0;
int contadorDec = 0;
int contadorDec2 = 0;
int temp;
int contadorNum2 = 0;
int contadorDec3 = 0;
int contadorDec4 = 0;

int DatoRecibido;
int contadorUART = 0;
int espacio = 32;
int borrar = 8;
int banderaUART1 = 1;

//******************************************************************************
// Vector de interrupcion
//******************************************************************************

void __interrupt() ISR(void){
//****************************************************************
//      INTERRUPCION UART
//**************************************************************** 
    InterruptReciboUSART(&DatoRecibido);
    
    if (DatoRecibido == 43){
        contadorUART = contadorUART + 1;
        DatoRecibido = 32;
    }
    if (DatoRecibido == 45){
        contadorUART = contadorUART - 1;
        DatoRecibido = 32;
    }
}
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void Setup(void);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void) {
    Setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
        EnvioSerial(borrar);  //Borramos la consola para que no se repitan valores o se sature la consola
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);
        EnvioSerial(borrar);  
        
        
       PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(1);
       
//******************************************************************************
//  CONVERSION ADC       
//******************************************************************************      
       estado = 0;
       spiWrite(estado);
       temp_ADC1 = spiRead();
       
       __delay_ms(100);    
       
       estado = 1;
       spiWrite(estado);
       temp_ADC2 = spiRead();
  
       
//******************************************************************************
//  CONTADOR LCD    
//******************************************************************************
        PORTD = temp_ADC2;
        PORTB = temp_ADC1;
        
        
        __delay_ms(5); //Imprimir en LCD la parte entera de la division:
//******************************************************************************
//  COMUNICACION MASTER/CPU                
//******************************************************************************         
    temp = temp_ADC1;
    
        while (temp > 0){
            temp = temp - 51;
            contadorNum2 = contadorNum2 + 1; 
            if (temp <= 0){
                break;
            }
        }
        if (temp == 0){
            EnvioSerial(VOLTAJE[contadorNum2]);
            contadorNum2 = 0;
            __delay_ms(5); //Imprimir en LCD la parte entera de la division:
            EnvioSerial('.');
            __delay_ms(5);  //Imprimir punto decimal
            EnvioSerial('0');
            __delay_ms(5);  //Imprimir punto decimal
            EnvioSerial('0');
            __delay_ms(5);  //Imprimir punto decimal
        }
        if (temp < 0){
            temp = temp + 51;
            contadorNum2 = contadorNum2 - 1;
            EnvioSerial(VOLTAJE[contadorNum2]);
            contadorNum2 = 0;
            __delay_ms(5); //Imprimir en LCD la parte entera de la division:
            EnvioSerial('.'); 
            __delay_ms(5);  //Imprimir punto decimal
            temp = temp * 10;
//*******************************************************************************            
            while (temp > 0){
            temp = temp - 51;
            contadorDec3 = contadorDec3 + 1;
                if (temp <= 0){
                    break;
                }
            }
            if (temp == 0){
            EnvioSerial(VOLTAJE[contadorDec3]);
            contadorDec3 = 0;
            __delay_ms(5);  //Imprimir en LCD unico decimal de la division:
            EnvioSerial('0');
            __delay_ms(5);  //Imprimir punto decimal
            }
            if (temp < 0){
            temp = temp + 51;
            contadorDec3 = contadorDec3 - 1;
            EnvioSerial(VOLTAJE[contadorDec3]); 
            contadorDec3 = 0;
            __delay_ms(5);  //Imprimir en LCD el primer decimal de la division:
            temp = temp * 10;
//*******************************************************************************              
                while (temp > 0){
                temp = temp - 51;
                contadorDec4 = contadorDec4 + 1;
                    if (temp <= 0){
                        break;
                    }
                }
                if (temp == 0){
                EnvioSerial(VOLTAJE[contadorDec4]);
                contadorDec4 = 0;
                __delay_ms(5); //Imprimir en LCD unicos dos decimales de la division: 
                }
                if (temp < 0){
                temp = temp + 51;
                contadorDec4 = contadorDec4 - 1;
                EnvioSerial(VOLTAJE[contadorDec4]);
                contadorDec4= 0;
                 __delay_ms(5);  //Imprimir en LCD el segundo decimal de la division:
                }
            }
        }
        EnvioSerial('V');
        EnvioSerial(espacio);
       
       __delay_ms(1);
       PORTCbits.RC2 = 1;       //Slave Deselect 
       
       __delay_ms(250);
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void Setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISC2 = 0;
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    PORTCbits.RC2 = 1;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    SPBRG = 12; //baud rate 9600
    SYNC = 0; //comunicación asíncrona
    SPEN = 1; //habilita comunicación
    CREN = 1;
    TXEN = 1;
    //Limpiamos bandera de recepción
    RCIF = 0;
    RCIE = 1; //Habilita interrupción de recepción
    
    PEIE=1;     //Habilita interrupciones perifericas
    
    GIE=1;    //Habilita interrupciones globales
    
    IRCF0 = 1; //Oscilador a 8MHz
    IRCF1 = 1;
    IRCF2 = 1;
    SCS = 1;

}