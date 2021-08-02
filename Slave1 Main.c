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
#include "ADC header.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

char Low_ADC1;
char Low_ADC2;
char High_ADC1;
char High_ADC2;
char temp;
char temp1;
char DisplayLow_ADC;
char DisplayHigh_ADC;
char contador_ADC;

char LecturaSPI;
char estado_ADC;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
//****************************************************************
//      INTERRUPCION ADC
//****************************************************************    
    InterruptADC(&DisplayLow_ADC, &DisplayHigh_ADC);        //Funcion para saber que fue lo que se convirtio del ADC
    
    if (contador_ADC == 1) {                                //Escogemos puerto 
        High_ADC1 = DisplayHigh_ADC;
        Low_ADC1 = DisplayLow_ADC;
    }
    
    if (contador_ADC == 0) {
        High_ADC2 = DisplayHigh_ADC;
        Low_ADC2 = DisplayLow_ADC;
    }
//****************************************************************
//      INTERRUPCION SPI PARA RECIBIR
//****************************************************************     
   if(SSPIF == 1){
        LecturaSPI = spiRead();
        
        SSPIF = 0;
    }
}
//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
//******************************************************************************
//  CONVERSION ADC       
//******************************************************************************
        contador_ADC = 1;   //Indicamos en que puerto mostraremos nuestra conversion
        Read_ADC(2);        //Hacemos una conversion usando canal 13
        
        __delay_ms(5);
        
        contador_ADC = 0;   
        Read_ADC(3);       //Hacemos una conversion usando canal 11
        
//******************************************************************************
//  CONVERSION ADC       
//******************************************************************************
        
        if (LecturaSPI == 1) {
            temp = (High_ADC1 << 4) + Low_ADC1;
            PORTB = temp;
            spiWrite(temp);
        }
        if (LecturaSPI == 0) {
            temp1 = (High_ADC2 << 4) + Low_ADC2;
            PORTD = temp1;
            spiWrite(temp1);
        }
        
            
            
            
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    
    TRISA = 0xFF; 
    TRISD = 0x00;
    TRISB = 0x00;
       
    PORTA = 0;
    PORTB = 0;     
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    ANSEL = 0;
    ANSELH = 0;
    ANS2 = 1; //Indicamos que pines son analogicos y cuales digitales del puerto A
    ANS3 = 1;

    ADCON0bits.ADCS = 2;
    ADCON0bits.GO = 0;
    ADCON0bits.ADON = 1;
    ADCON1 = 0x80;
    ADIF = 0;   //Habilita interrupcion del adc
    ADIE = 1;
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones perifericas
    
    
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    IRCF0 = 1; //Oscilador a 8MHz
    IRCF1 = 1;
    IRCF2 = 1;
    SCS = 1;
}