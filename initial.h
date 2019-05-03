/* 
 * File:   initial.h
 * Author: YEN
 *
 * Created on 2017?4?11?, ?? 3:51
 */

#ifndef INITIAL_H
#define	INITIAL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
//#define M_866
//#define M_910
//#define DEBUG
#define del_ok
#define send_ok
//--------------------
    
#define OUTPUT  0
#define INPUT   1
   
//PORTA
#define BAT_DECT        PORTAbits.RA0
#define BAT_DECT_TRIS   TRISAbits.TRISA0
     
#define MD_RESET        LATAbits.LATA1
#define MD_RESET_TRIS   TRISAbits.TRISA1
    
#define MD_START        LATAbits.LATA2
#define MD_START_TRIS   TRISAbits.TRISA2
    
#define MD_POWER        LATAbits.LATA3
#define MD_POWER_TRIS   TRISAbits.TRISA3

//PORTC
#define VER_SELECT      LATCbits.LATC1
#define VER_SELECT_TRIS TRISCbits.TRISC1    
#define SPK             LATCbits.LATC3
#define SPK_TRIS        TRISCbits.TRISC3

#define LED_B           LATCbits.LATC4
#define LED_B_TRIS      TRISCbits.TRISC4
#define LED_RX_OUT      LATCbits.LATC5
#define LED_RX_OUT_TRIS TRISCbits.TRISC5
#define MD_RX           PORTCbits.RC6
#define MD_RX_TRIS      TRISCbits.TRISC6
#define MD_TX           LATCbits.LATC7
#define MD_TX_TRIS      TRISCbits.TRISC7
    
//PORTB
#define STANDBY_PIN         PORTBbits.RB0
#define STANDBY_PIN_TRIS    TRISBbits.TRISB0    
#define ALARM_PIN           PORTBbits.RB1
#define ALARM_PIN_TRIS      TRISBbits.TRISB1     
#define TAMPER_PIN         PORTBbits.RB0
#define TAMPER_PIN_TRIS    TRISBbits.TRISB0    
#define LEARN_PIN           PORTBbits.RB1
#define LEARN_PIN_TRIS      TRISBbits.TRISB1

#define ERROR_PIN           PORTBbits.RB2
#define ERROR_PIN_TRIS      TRISBbits.TRISB2    

#define LED_G                 LATBbits.LATB3
#define LED_G_TRIS            TRISBbits.TRISB3
   
#define TEST_PIN            PORTBbits.RB4
#define TEST_PIN_TRIS       TRISBbits.TRISB4
#define LED_RX_IN                 LATBbits.LATB4
#define LED_RX_IN_TRIS            TRISBbits.TRISB4
    
#define WIFI_INT             PORTBbits.RB5
#define WIFI_INT_TRIS        TRISBbits.TRISB5
#define WIFI_RX             LATBbits.LATB6
#define WIFI_RX_TRIS        TRISBbits.TRISB6
#define WIFI_TX             PORTBbits.RB7
#define WIFI_TX_TRIS        TRISBbits.TRISB7

//-----------------
#define HIGH_FREQ_OSCCON 0x68   
#define HIGH_FREQ_T0CON 0x87
#define HIGH_FREQ_TMR0  781     //4M->390       (1/8M)*4*256*781=99.9968ms
//-----    
#define LOW_FREQ_OSCCON 0x00   
#define LOW_FREQ_T0CON 0x80
#define LOW_FREQ_TMR0  383      //360           (1/31.25K)*4*2*390=99.84ms
//--------------------
#define START_MM_COUNT 0x00
#define END_MM_COUNT 0xA0
#define RETRY_NUMBER1 	(5-1)
#define RETRY_NUMBER2 	(3-1)    
#define BUFFER_STACK    6
#define LOG_MAX_T   220    
//-------------------- 
#define SMOKE_ALARM_T   0    
#define TEST_CYCLE_S   1
#define LOW_BATTERY_T   2    
#define TAMPER_OPEN_T   3
#define TAMPER_CLOSE_T   4    
#define TEST_PIN_T   5
#define LOW_BATTERY_S   6      
#define EOL_T   7
#define FLOOD_T   8
#define GLASS_T   9
#define SUPERVISORY_T   10
#define CARBON_T 11
#define MOTION_T 12
#define DOOR_T 13
#define PANIC_T 14
#define HVAC_T 15
#define APPLIANCE_T 16
#define RESERVE1_T 17
#define RESERVE2_T 18
#define RESERVE3_T 19
#define RESERVE4_T 20
#define RESERVE5_T 21
#define RESERVE6_T 22
#define RESERVE7_T 23   
#define GO_SMS_T 24    
//--------------------     
#define LED_OFF 0
#define LED_STANDBY 1
#define LED_INTERNET 2
#define LED_NO_SET 3
#define LED_NET_ERR 4
#define LED_REGISTER 5
#define LED_RF_ADD 6 
#define LED_RF_DEL 7 
#define MD_POWER_LOSS 8 
//--------------------       
#define EVEN_MAX 50
#define BT_EOL_RESEND 1 //DAY
//--------------------
//#ifdef M_866
    #define POWER_ON  0   
    #define POWER_OFF 1
/*#else
    #define POWER_ON  0    
    #define POWER_OFF 1
#endif*/

#define EMS31   0
#define PLS8    1
#define LE910   2
#define LE866   3
#define EHS5    4

#define GEMALTO     (Module_type==EMS31||Module_type==PLS8||Module_type==EHS5)
#define TELIT       (Module_type==LE910||Module_type==LE866)
    
#define KEY_NONE 0
#define KEY_IN_LEARN 1
#define KEY_ADD_ID 2
#define KEY_DEL_ID 3
    
#define SMOKER 0
#define SUPER_HUB 1
//--------------------
unsigned char const VERSION[]="2042";
//--------------------       
bit HL_freq = 0;
//-------------------- 
unsigned char const encryption_code[16]={ 0x6c,0x31,0x6e,0x79,0x52,0x7f,0x26,0x6f,
										 0x34,0x19,0x41,0x1b,0x54,0x3a,0x64,0x1f };
    
//--------------------
unsigned char encryption = 0;
unsigned char rsp_buffer[250];
unsigned char enc_cnt;
unsigned char stack_buffer[BUFFER_STACK][LOG_MAX_T];
unsigned char random_rx;

unsigned char even_log[EVEN_MAX][2];
unsigned char even_count_f = 0;
unsigned char even_count_l = 0;

#ifdef DEBUG
unsigned char debug_buffer[250];
#endif
unsigned char debug_p;
unsigned char signal_val;
unsigned char key_p,key[100];
unsigned char back_door;
unsigned char respond_day = 0;
unsigned char LED_flash_type = 0;
unsigned int retry_count = 0;
unsigned char IP_type = 0;
unsigned char test_enable=0;
unsigned int ADC_data = 0;

unsigned char RF_wait_count = 0;


unsigned char ID_LIST[28][9];
unsigned char learning_mode = 0;
unsigned char power_status=0;
unsigned char chk_supervisory = 0;

unsigned char Module_type = EMS31;
unsigned char ver_select = 0;





void Uart_initial_BD2(void);
void Uart_disable(void);
void Uart_initial_115200(void);
void ADC_init(void);
void Uart_initial(void);
void IO_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* INITIAL_H */

