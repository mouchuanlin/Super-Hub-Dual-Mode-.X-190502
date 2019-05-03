#include "initial.h"
#include <pic18f26k22.h>
#include <xc.h>

void IO_init(void)
{       
//PORTA
    ANSELA = 0x01;
    BAT_DECT = 1;
    BAT_DECT_TRIS = INPUT;
    
    MD_RESET = 1;
    MD_RESET_TRIS = OUTPUT;   
    
    MD_START = 0;
    MD_START_TRIS = OUTPUT;
    MD_POWER = 1;   //-----
    MD_POWER_TRIS = OUTPUT;

//PORTC
    ANSELC = 0x00;    
    SPK = 1;
    // mlin - beeping
    //SPK_TRIS = OUTPUT;    
    VER_SELECT = 1;
    VER_SELECT_TRIS = INPUT; 
    LED_B = 1;
    LED_B_TRIS = OUTPUT;
    LED_RX_OUT = 1;      
    LED_RX_OUT_TRIS = OUTPUT;            
    LED_G = 1;      
    LED_G_TRIS = OUTPUT;
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
    
//PORTB
    ANSELB = 0x00;    
    TAMPER_PIN = 1;
    TAMPER_PIN_TRIS = INPUT;
    LEARN_PIN = 1;
    LEARN_PIN_TRIS = INPUT;
    ERROR_PIN = 1;
    ERROR_PIN_TRIS = INPUT;

    LED_RX_IN = 1;
    LED_RX_IN_TRIS = OUTPUT;
    WIFI_INT = 1;
    WIFI_INT_TRIS = INPUT;         
    WIFI_RX = 1;
    WIFI_RX_TRIS = OUTPUT;
    WIFI_TX = 1;
    WIFI_TX_TRIS = INPUT;
    
}

//---------------------------------------------------
void Uart_initial(void)
{
    TX1STA = 0x26;       //00100110
    RC1STA = 0x90;       //10010000
    BAUDCON1 = 0xc8;     //11001000
    SPBRG1 = 103;       //19200bps
    SPBRGH1 = 0;
    
    TX2STA = 0x26;       //00100110
    RC2STA = 0x90;       //10010000
    BAUDCON2 = 0xc8;     //11001000
    SPBRG2 = 207;       //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
}

//---------------------------------------------------
void Uart_initial_BD2(void)
{   
    TX2STA = 0x26;       //00100110
    RC2STA = 0x90;       //10010000
    BAUDCON2 = 0xc8;     //11001000
    SPBRG2 = 207;       //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
}

//---------------------------------------------------
void Uart_initial_115200(void)
{
    TX1STA = 0x26;       //00100110
    RC1STA = 0x90;       //10010000
    BAUDCON1 = 0xc8;     //11001000
    SPBRG1 = 16;
    SPBRGH1 = 0;
}

//---------------------------------------------------
void Uart_disable(void)
{
    RC1STA = 0x00;       //10010000
    MD_RX = 0;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
    RC1IE = 0;
    
    RC2STA = 0x00;       //10010000
   // WIFI_RX = 0;
  //  WIFI_RX_TRIS = OUTPUT;
   // WIFI_TX = 0;
   // WIFI_TX_TRIS = OUTPUT;
    RC2IE = 0;
}

//---------------------------------------------------
void Enable_Uart_IO(void)
{
    MD_RX = 1;
    MD_RX_TRIS = OUTPUT;
    MD_TX = 1;
    MD_TX_TRIS = INPUT;
}
//---------------------------------------------------
void ADC_init(void)
{
    ADCON0 = 0b01111100;
    ADCON1 = 0b00000000;
    ADCON2 = 0b10101111;
    VREFCON0 = 0b00100000;
}

