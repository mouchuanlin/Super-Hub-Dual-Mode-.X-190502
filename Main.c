/* 
 * File:   Main.c
 * Author: YEN
 *
 * Created on 2017?4?11?, ?? 3:13
 */

// PIC18F26K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = SWON      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 16    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = INTMCLR  // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = ON        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = ON        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = ON        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = ON        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = ON        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = ON        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <stdio.h>
#include <stdlib.h>
#include <pic18f26k22.h>
#include <xc.h>
#include "System_Library.h"
#include "initial.h"
#include "EE_library.h"
#include "SMS_library.h"
#include "Module_Library.h"
#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include "emc_library.h"

    //unsigned char Alarm_f = 0;
    unsigned char Standby_f = 0;
    unsigned char Error_f = 0;
  //  unsigned char Test_f = 0;
  //  bit Low_battery = 0;
   // bit Smoke_EOL = 0;
    bit Test_click = 0;        
    unsigned char Smoke_respond = 0;    
    bit ADC_time = 0;
    //bit Respond_ack = 0;
    unsigned char BT_S_respond = 0;
    unsigned char BT_L_respond = 0;
    unsigned char EOL_respond = 0;
    //bit Tamper_open_f = 0;
    //bit Tamper_close_f = 0;    
    unsigned int test_count = 0;
    unsigned int first_test = 18000; //30*60*10 (30mins)
    
    unsigned char error_count = 0;
    unsigned int standby_count = 0;
    unsigned int error_time_detect = 0;
    unsigned int test_time_detect = 0;
    unsigned int adc_count = 0;
    unsigned char Respond_T_Hour = 0;
    unsigned char Respond_T_Day = 0;
    unsigned char err_count=0;
    unsigned char LED_count = 0;
    unsigned char error_status_count = 0;
    unsigned char alarm_count = 0;
    unsigned char rx2_cnt = 0;
    unsigned char rx2_buf[20];
    unsigned int exit_learn;
    unsigned char tamper_status = 1;
    unsigned char first_tamper = 1;
    unsigned char led_count = 0;
    unsigned char learn_delay = 0;    
    unsigned int test_9sec_count=0;
    unsigned char OTA_flag = 0;
#define MODULE_OFF_TYPE   
/*
 * 
 */
    
int main(int argc, char** argv) {

    unsigned char cnt,rsp,temp;
    unsigned char module_start_cnt;
    unsigned char sms_time,WDT_count;
    // mlin - REGISTER 2-1: OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCON = HIGH_FREQ_OSCCON;	// 8MHz
    //MCLRE = 0;
    HL_freq = 1;
    SWDTEN = 0;     

    IO_init();
    Uart_initial();      
    ADC_init();
    
    T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
    TMR0L = ((65535-HIGH_FREQ_TMR0)%256);
    TMR0H = ((65535-HIGH_FREQ_TMR0)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    
    INTEDG0 = 1;
    INTEDG1 = 1;
    INTEDG2 = 1;
    INT1IP = 1;     //High Priority
    INT2IP = 1;     //High Priority
    IOCB = 0x10;    //RB.4 IOC enable
    
    INTEDG0 = 0;    //Int0 falling edge
    INTEDG1 = 1;    //Int1 rising edge
    INT0IE = 1;     //Enable Int0 interrupt
    INT1IE = 1;     //Enable Int1 interrupt
    //INT2IE = 1;     //Enable Int2 interrupt
    RBIE = 1;       //Enable IOC interrupt
    RC1IE = 1;      //Enable UART0_RX interrupt
    PEIE = 1;       //Enable LOW&HIGH Priority interrupt    
    
    for( rsp=0;rsp<BUFFER_STACK;rsp++ )
    {
        cnt = 0;
        do{
            stack_buffer[rsp][cnt] = 0x00;
        }while(++cnt<LOG_MAX_T);
    }
    test_count = 0;
    module_start_cnt = 0;
    learning_mode = KEY_NONE;
    ADC_time = 1;
    // Test_click = 1;
    
    loed_ID_to_buffer();
    
    if( VER_SELECT==1 ) //Smoker
    {
        ver_select = SMOKER;
        TEST_PIN = 1;
        TEST_PIN_TRIS = INPUT;
        INT2IE = 1;     //Enable Int2 interrupt
    }else{              //super hub
        ver_select = SUPER_HUB;   
        LED_RX_IN = 1;
        LED_RX_IN_TRIS = OUTPUT;
        INT2IE = 0;     //Disable Int2 interrupt
    }
    cnt = 10;
    do{
        SPK = 1;
        delay5ms(20);
        SPK = 0;
        delay5ms(20);
        CLRWDT();
    }while(--cnt!=0);
  /*  GIE = 1;
    SWDTEN = 1;
    do{
       // SWDTEN = 1;
        SLEEP();   
        //CLRWDT();
        //SWDTEN = 0;
        if( ++WDT_count>=10 )
        {
            LED_G = ~LED_G;
            WDT_count = 0;
        }
    }while(1);*/
    GIE = 1;        //Enable HIGH Priority interrupt   
//========================Module start(S)========================
module_start:    
    SWDTEN = 1;
    RF_wait_count = 0;
    LED_flash_type = LED_REGISTER;
    OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
    T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
    TMR0IF = 0;
    TMR0IE = 1;
    TMR0ON = 1;
    HL_freq = 1;
//    IO_init();
    Enable_Uart_IO();
    Uart_initial(); 
    MD_POWER = POWER_ON;
    module_start_cnt++;
    delay5ms(200);
    delay5ms(200); 
    MD_RESET = 1;
    delay5ms(200); 
    MD_RESET = 0;   
    delay5ms(200); 
    delay5ms(200); 
        
    MD_START = 0;
    delay5ms(200);
    delay5ms(200);
    delay5ms(200);
    MD_START = 1;   
    cnt = 25;
    do{
        delay5ms(200);
        CLRWDT();
    }while(--cnt!=0);
    MD_START = 0;
    first_run();   
    load_default();
   /* do{
        CLRWDT();        
    }while(1);*/;
    //----------- wait 10sec ----------
    cnt = 10;
        //while(cnt!=0);            //Firmware update
    do{
        delay5ms(200);
     //   LED = ~LED;
    }while( --cnt!=0 );       
    //---- wait AT command respond-----
    rsp = 15;
    do{         
        Uart_initial_115200();
        soutdata("AT+IPR=115200\r\n$");
        wait_ok_respond(40);
        //Uart_initial();
        cnt = check_module_run();
        if( cnt!='K' )
        {
            delay5ms(250);
            delay5ms(250);
            delay5ms(250);    
            Uart_initial_115200();
            cnt = check_module_version(1);        
            if( cnt=='K' )
            {
                if( Module_type == PLS8 )
                    cnt = 'K';
                else cnt = 'E';
            }
        }
        CLRWDT();
    }while( --rsp!=0&&cnt!='K' );     
    if( rsp==0 )
    {        
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
        cnt = 30;
        do{
           delay5ms(200);
        }while(--cnt!=0);
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        power_status = MD_POWER_LOSS;
        goto module_start;
    }   
    //---------------------------------   
    wait_ok_respond(40);
    cnt = 10;
    do{
        rsp = check_module_version(cnt);
        if( rsp=='E' )
            power_status = MD_POWER_LOSS;
        else power_status = 0;
        CLRWDT();
    }while(--cnt!=0&&rsp=='E');
    wait_ok_respond(40);
    if( GEMALTO )
        GM_module_first_run();
    else TL_module_first_run();    
    //--------------------------------
    delay5ms(100);
    soutdata("AT+VER=$");
    if( VER_SELECT==1 ) //Smoker
    {
        soutdata("Smoker,$");
    }else soutdata("Super HUB,$");
    if( Module_type==EMS31 )    
        soutdata("EMS31\r\n$");
    else if( Module_type==PLS8 )    
        soutdata("PLS8\r\n$");    
    else if( Module_type==LE910 )    
        soutdata("LE910\r\n$");    
    else //if( Module_type==LE866 )    
        soutdata("LE866\r\n$");    
    delay5ms(200);
        
    //-------- wait SIM ready---------
    cnt = 50;
    do{       
        rsp = check_sim_card();
        if( rsp=='N'&&cnt==30 )        
         {
            #ifdef MODULE_OFF_TYPE
                MD_POWER = POWER_OFF;
            #else
                MD_RESET = 1;
            #endif
            cnt = 30;
            do{
                delay5ms(200);
            }while(--cnt!=0);
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
            power_status = MD_POWER_LOSS;
            goto module_start;
        } 
        CLRWDT();
    }while( rsp!='K'&& --cnt!=0 );
    if( cnt==0 )    
    {
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
        cnt = 30;
        do{
           delay5ms(200);
        }while(--cnt!=0);
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        goto module_start;
    }
    //-------- wait register network---------
    cnt = 50;
    do{
        if( cnt%2==0 )
            rsp = check_register(0);
        else 
            rsp = check_register(1);
        CLRWDT();
    }while( rsp!='K' && --cnt!=0 );
    if( cnt==0 )    
    {
        #ifdef MODULE_OFF_TYPE
            MD_POWER = POWER_OFF;
        #else
            MD_RESET = 1;
        #endif
        cnt = 30;
        do{
           delay5ms(200);
        }while(--cnt!=0);
        #ifndef MODULE_OFF_TYPE
            MD_RESET = 0;
        #endif
        goto module_start;
    }
    power_status = 0;
    delay5ms(100);
    check_csq();
    check_led_type();
    soutdata("AT\\Q0\r\n$");
    //OTA_flag = 1; //----------------------------------
    //--------- Alarm or Report ---------    
alarm_start:    
    if( LED_flash_type==LED_STANDBY )
    {
        LED_flash_type = LED_INTERNET;
        rsp = check_emc_stack();
        if( rsp=='U' )
        {
            #ifdef MODULE_OFF_TYPE
                MD_POWER = POWER_OFF;
             #else
                MD_RESET = 1;
            #endif
            cnt = 30;
            do{
                delay5ms(200);
            }while(--cnt!=0);
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
            goto module_start;
        }
        
        if( OTA_flag==1 )
        {
            rsp = Check_OTA();
            if( rsp=='E' )
                OTA_flag = 2;
            else OTA_flag = 0;
        }
    }
    check_led_type();
    //--------- Wait SMS Setting ---------
    if( Test_click==1 )    
    {
        sms_time = read_ee(0x00,0xB8);   //wait SMS time
        set_sms_init();    
        do{
            cnt = 12;
            do{
                rsp = 16;
                do{
                    delay5ms(200);    
                    if( even_count_f!=even_count_l)     
                        check_even(); 
                    if( stack_buffer[0][0]!=0&&retry_count==0&&IP_type==1 )   
                    {                   
                       goto alarm_start;
                    }
                }while(--rsp!=0);
              //  LED = 0;
                check_sms();
                delay5ms(100);
                if( check_csq()==0 )
                {
                    Uart_initial_115200();
                    //soutdata("AT+IPR=19200\r\n$");
                    //wait_ok_respond(40);
                    //Uart_initial();
                    set_sms_init();    
                }
              //  LED = 1;
            }while(--cnt!=0);      
            CLRWDT();
        }while(--sms_time!=0);
        Test_click = 0;
    }
    if( even_count_f!=even_count_l)     
        check_even();
    if( stack_buffer[0][0]!=0&&retry_count==0&&IP_type==1 )//LED_flash_type==LED_STANDBY )                     
        goto alarm_start;
//========================Module start(E)========================          
    LED_RX_IN = 1;
    module_start_cnt = 0;   
    LED_flash_type = LED_OFF;
    #ifdef MODULE_OFF_TYPE
        MD_POWER = POWER_OFF;
    #else
        MD_RESET = 1;
        delay5ms(200);   
        MD_RESET = 0;
    #endif
    Uart_disable();
   // OSCCON = LOW_FREQ_OSCCON;
   // HL_freq = 0;
    //T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
    //TMR0L = ((65535-LOW_FREQ_TMR0)%256);
    //TMR0H = ((65535-LOW_FREQ_TMR0)/256);
    TMR0IF = 0;
    TMR0IE = 0;//
    TMR0ON = 0;
    PEIE = 1;
    GIE = 1;      
    SWDTEN = 0;
    do{
        //LED_flash_type = LED_OFF;
        if( RF_wait_count==0)
        {
            SWDTEN = 1;
            SLEEP();   
            NOP();
            NOP();
            //SWDTEN = 0;
            if( ++WDT_count>=3 )
            {            
                WDT_count = 0;
                //LED_G = ~LED_G;
                if( first_test!=0 )
                {
                    first_test--;
                    if( first_test!=0 )
                        first_test--;
                }                

                if( VER_SELECT==1&&TEST_PIN==1 )
                {
                    if( ++test_9sec_count==45 )   //100ms*90
                        add_even(TEST_PIN_T,0);
                }else test_9sec_count = 0;
        
                if( ++adc_count>=(18000*0.9888) )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
                {            
                    chk_supervisory++;    //----add supervisory
                    adc_count = 0;   
                    if( ++Respond_T_Hour>=24 )       //24
                    {
                        Respond_T_Hour = 0;
                        Respond_T_Day++;
                        if( Respond_T_Day>= respond_day )         
                        {
                            Respond_T_Day = 0;
                            add_even(TEST_CYCLE_S,0);             
                            rsp_SUP_LBT();
                            OTA_flag = 1;
                        }
                        if( BT_S_respond!=0 )
                            BT_S_respond--;
                        if( BT_L_respond!=0 )
                            BT_L_respond--;
                        if( EOL_respond!=0 )
                            EOL_respond--;
                        if( OTA_flag==2 )
                            OTA_flag = 1;
                    }            
                }else if( (adc_count%300)==0 )        //every 10 mins check low battery
                    ADC_time = 1;
               
                if( retry_count!=0 )
                {
                    retry_count--;
                    if( retry_count!=0 )               
                        retry_count--;
                }
        
                if( led_count!=0 )
                {
                    if( --led_count==0 )
                    {
                        LED_RX_IN = 1;
                        LED_RX_OUT = 1;
                    }else{
                        if( --led_count==0 )
                        {
                            LED_RX_IN = 1;
                            LED_RX_OUT = 1;
                        }                        
                    }
                }
            
                if( tamper_status!=0&&TAMPER_PIN==1  )
                {
                    if( ++tamper_status>2 )
                    {
                        if( first_tamper==0 )                    
                            add_even(TAMPER_CLOSE_T,1);
                        first_tamper = 0;
                        tamper_status = 0;
                        SPK = 1;
                    }
                }   
        
                if( learn_delay!=0 )
                {
                    learn_delay--;
                    if( learn_delay!=0 )            
                        learn_delay--;                    
                }
            
                if( learning_mode == KEY_NONE )
                {                
                    if( retry_count==0 )
                    {
                        LED_B = 1;
                        LED_G = 1;
                    }else       //new add V104
                    {
                        LED_count++;
                        if( LED_count < 2 )                
                        {
                            LED_G = 0;     
                            LED_B = 0;
                        }
                        else
                        {
                            LED_G = 1;
                            LED_B = 1;
                        }                
                        if( LED_count>=25 )
                            LED_count = 0;
                    }
                
                }else{  
                    if( ++exit_learn>=300 )     //100*100ms=10Sec
                        learning_mode = KEY_NONE;
                    if( learning_mode==KEY_IN_LEARN)
                    {
                        LED_B = 0;
                        LED_G = 0;
                    }else if( learning_mode==KEY_ADD_ID )
                    {
                        LED_count++;
                        if( LED_count < 2 )                
                            LED_B = 0;     
                        else LED_B = 1;
                        LED_G = 0;
                        if( LED_count>=10 )
                            LED_count = 0;
                    }
                    else if( learning_mode==KEY_DEL_ID )
                    {
                        LED_count++;
                        if( LED_count < 2 )                
                            LED_G = 0;     
                        else LED_G = 1;
                        LED_B = 0;
                        if( LED_count>=10 )
                            LED_count = 0;          
                    }
                }
                if( ver_select == SMOKER )
                {
                    if( alarm_count!=0 )
                    {
                        if( ALARM_PIN==1 )
                        {
                            if( ++alarm_count>=5 )
                            {
                                add_even(SMOKE_ALARM_T,1); 
                            // send_trigger_to_RF(1);
                                alarm_count = 0;
                            }
                        }else alarm_count = 0;
                    }
                    if( ERROR_PIN==1 )
                    {
                        if( ++err_count>=5 )   //100ms*10 = 1 sec
                        {
                            if( err_count==5 )
                            {
                            //   if( alarm_out(TAMPER_OPEN_T)==0 )
                            //      Tamper_open_f = 1;
                                add_even(TAMPER_OPEN_T,1);
                            }
                            err_count = 5;
                        }
                   }else
                    {
                        if( err_count>=5 )
                        {
                          //  if( alarm_out(TAMPER_CLOSE_T)==0 )
                            //    Tamper_close_f = 1;
                                add_even(TAMPER_CLOSE_T,1);
                        }
                        err_count = 0;
                    }   
        
                    if( Standby_f==1 )
                    {
                        Standby_f = 0;
                        standby_count = 0;
                    }else{
                        standby_count++;            //100ms*4000=400Sec
                        if( standby_count>= 2000)
                        {
                            standby_count = 0;
                            Smoke_respond = 1;
                        }
                    }
        
                    if( Error_f==1 )
                    {
                        Error_f = 0;
                        error_count++;
                        error_time_detect = 0;
                    }else
                    {                
                        if( error_count!=0 )
                        {
                            if( ++error_time_detect>=150 )  //100ms*300=30sec
                            {                    
                                if( error_count==1 )            
                                {
                                    if( ++error_status_count>=5 )
                                    {
                                        if( BT_S_respond==0 )
                                        {
                                            add_even(LOW_BATTERY_T,1);
                                            error_status_count = 0;
                                            BT_S_respond = BT_EOL_RESEND;
                                        }
                                    }   
                                }else if( error_count==3 )
                                {
                                    if( ++error_status_count>=5 )
                                    {
                                        if( EOL_respond==0 )
                                        {
                                            add_even(EOL_T,1);
                                            error_status_count = 0;
                                            EOL_respond = BT_EOL_RESEND;
                                        }
                                    }
                                }else error_status_count = 0;
                                error_count = 0;
                                error_time_detect = 0;                                            
                            }   
                        }else
                        {
                            if( ++error_time_detect>=150 )  //100ms*300=30sec
                            { 
                                error_time_detect = 0;   
                                error_status_count = 0;                    
                            }
                        }
                    } 
                }             
                if( test_count!=0 )
                {
                    test_time_detect++;
                    if( learning_mode == KEY_NONE )
                    {
                        if( test_count==5&&test_time_detect>5 ) //1sec
                        {
                            learning_mode = KEY_IN_LEARN;
                            exit_learn = 0;
                            test_count = 0;
                            test_time_detect = 0;
                        }else if( test_time_detect>=10 )  //100ms*20=2sec
                        {
                            test_count = 0;
                            test_time_detect = 0;
                        }
                    }else{                                                        
                        if( ++test_time_detect>=10 )  //100ms*20=2sec
                        {               
                            if( test_count==1 )
                            {
                                Test_click = 1;
                                add_even(GO_SMS_T,0);
                                learning_mode = KEY_NONE;
                            }else if( test_count==2 )
                            {
                                learning_mode = KEY_ADD_ID;                        
                            }else if( test_count==3 )
                            {
                                learning_mode = KEY_DEL_ID;                   
                            }else if( test_count==4 )
                            {
                                add_even(TEST_PIN_T,0);
                             //   send_trigger_to_RF(0);
                                learning_mode = KEY_NONE;
                            }else if( test_count==5 )
                            {
                                learning_mode = KEY_NONE;
                            }                         
                            test_count = 0;
                            test_time_detect = 0;
                        }
                    }           
                }
                if( RF_wait_count!=0 )
                {
                    LED_G = 0;
                    if( --RF_wait_count==0 )
                    {           
                     //   OSCCON = LOW_FREQ_OSCCON;
                     //   HL_freq = 0;
                        TMR0ON = 0;
                        //T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
                    }
                }
            }
        }
        CLRWDT();
        if( even_count_f!=even_count_l)    
        {
            check_even(); 
            retry_count = 0;        // new add on V104
        }
        
        if( Test_click==1 )
        {                     
            #ifdef MODULE_OFF_TYPE
               MD_POWER = POWER_OFF;
            #else
                MD_RESET = 1;
            #endif
            delay5ms(200);
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
            goto module_start;
        }
        
        if( ADC_time==1 )
        {
            FVREN = 1;
            cnt = 200;
            do{
                delay5ms(1);
                CLRWDT();
            }while(--cnt!=0&&FVRST==0);
            ADON = 1;            
            GO_nDONE = 1; 
            cnt = 200;
            do{
                delay5ms(1);
            }while( ((ADCON0&0x02)!=0)&&(--cnt!=0) );
            ADC_data = (ADRESH<<8)+ADRESL;
            ADC_data &= 0x03ff;
            ADON = 0;                               //748->2.80
            ADC_time = 0;                           //776->2.70
            if( ADC_data >776 )                     //806->2.60
            {                                       //838->2.50
                if( BT_L_respond==0 )
                {                    
                    add_even(LOW_BATTERY_S,0);                    
                    BT_L_respond = BT_EOL_RESEND;
                }
            }else BT_L_respond = 0;
            FVREN = 0;
        }
        
        if( WIFI_INT==1&&RF_wait_count==0 )
        {
            LED_G = 0;
            OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
            T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
            HL_freq = 1;
            Uart_initial_BD2();
            CREN1 = 0;
            RF_wait_count = 100;      
            TMR0IE = 1;//
            TMR0ON = 1;
            CREN1 = 1;
            RC2IE = 1;
        }
        
        if( chk_supervisory>=2 )       //2 hour
        {
            check_supervisory();
            chk_supervisory = 0;
        }
        
        if( retry_count==0&&stack_buffer[0][0]!=0&&IP_type==1 )        
        {                   
            #ifdef MODULE_OFF_TYPE
               MD_POWER = POWER_OFF;
            #else
                MD_RESET = 1;
            #endif
            delay5ms(200);
            #ifndef MODULE_OFF_TYPE
                MD_RESET = 0;
            #endif
            goto module_start;
        }           
    }while(1);
    goto module_start;    
    return (EXIT_SUCCESS);   
}

void interrupt tc_int( void )
{ 
/*    static unsigned char error_count = 0;
    static unsigned int standby_count = 0;
    static unsigned int error_time_detect = 0;
    static unsigned int test_time_detect = 0;
    static unsigned int adc_count = 0;
    static unsigned char Respond_T_Hour = 0;
    static unsigned char Respond_T_Day = 0;
    static unsigned char err_count=0;
    static unsigned char LED_count = 0;
    static unsigned char error_status_count = 0;
    static unsigned char alarm_count = 0;
    static unsigned char rx2_cnt = 0;
    static unsigned char rx2_buf[20];
    static unsigned int exit_learn;
    static unsigned char tamper_status = 1;
    static unsigned char first_tamper = 1;
    static unsigned char led_count = 0;
    static unsigned char learn_delay = 0;    
    static unsigned int test_9sec_count=0;*/
    unsigned char id[6];
    unsigned char zone,cnt;
    
    unsigned char temp;
    
    if( RC1IE==1&&RC1IF==1 )
    {
        do{
        //    LED = ~LED;		
            temp = RC1REG;
        }while(RC1IF==1);
       // RC1IF = 0;
    }
    
    if( RC2IE==1&&RC2IF==1 )
    {
        do{       	
            temp = RC2REG;
            //LED_G = 1;
            LED_B = 0;
            rx2_buf[rx2_cnt] = temp;
            if( ++rx2_cnt>=20 )
                rx2_cnt = 19;
            //out_sbuf2(rx2_cnt+0x30);
            if( temp=='\n' )
            {         
                if( rx2_cnt>=7 )
                {
                    cnt = 0;
                    if( rx2_buf[rx2_cnt-7]=='$'&&rx2_buf[rx2_cnt-2]=='\r')                    
                    {
                        for(zone=rx2_cnt-7;zone<rx2_cnt;zone++ )
                        {
                            rx2_buf[cnt++] = rx2_buf[zone];
                        }
                        rx2_cnt = 7;
                    }
                }
            /*    out_sbuf2('a');/////
                    out_sbuf2('t');//////
                   out_sbuf2('-');//////
                   for( zone=0;zone<rx2_cnt;zone++ )
                   {
                       temp = rx2_buf[zone];
                       out_sbuf2((temp>>4)+0x30);
                        out_sbuf2((temp&0x0f)+0x30);
                   }
                    out_sbuf2(0x0d);//
                    out_sbuf2(0x0a);//*/
                if( rx2_buf[0]=='$'&&rx2_buf[5]=='\r'&&rx2_buf[6]=='\n' )     //rf data in  $+3byte serial+1byte status+<CR>+<LF>                    
                {                
               //    out_sbuf2('a');/////
               //     out_sbuf2('t');//////
               //    out_sbuf2('-');//////
                    led_count = 10;
                    LED_RX_IN = 0;
                    rx2_cnt = 1;
                    do{
                        temp = (rx2_buf[rx2_cnt]>>4)&0x0f;
                        if( temp>=10 )
                        {
                            temp += 0x41;
                            temp -= 10;
                        }else temp += 0x30;
                        id[(rx2_cnt-1)*2] = temp;
                    //    out_sbuf2(temp);////
                        temp = rx2_buf[rx2_cnt]&0x0f;
                        if( temp>=10 )
                        {
                            temp += 0x41;
                            temp -= 10;
                        }else temp += 0x30;
                        id[(rx2_cnt-1)*2+1] = temp;
                    //    out_sbuf2(temp);//////
                    }while(++rx2_cnt<4); 
                    temp = rx2_buf[4];
                    //out_sbuf2((temp>>4)+0x30);
                    //out_sbuf2((temp&0x0f)+0x30);
                    //out_sbuf2(0x0d);//
                    //out_sbuf2(0x0a);//
                    //out_sbuf2('-');
                    zone = check_ID(&id);       //respond zone number(3~30),error=0
                   
                    if( zone!=0 )
                    {
                        ID_LIST[zone-3][7] = 0;     //clear supervisory count
                        LED_RX_OUT = 0;
                    }
                    if( learning_mode==KEY_NONE )
                    {
                        if( zone==0 )
                        {
                        /*    out_sbuf2('$');
                            out_sbuf2('A');
                            out_sbuf2('N');
                            out_sbuf2(0x0d);
                            out_sbuf2(0x0a);*/
                        }else{                      //Supervisory,nc,nc,low BT,nc,test,tamper,trigger/alarm                                                                                           
                            if( (rx2_buf[4]&0x01)!=0 )   //alarm
                            {                                                                                                
                                if( id[0]=='8')
                                    add_even(SMOKE_ALARM_T,zone);
                                else if( id[0]=='6' ) 
                                    add_even(FLOOD_T,zone); 
                                else if( id[0]=='2' )
                                    add_even(CARBON_T,zone);
                                else if( id[0]=='C' )
                                    add_even(GLASS_T,zone);
                                else if( id[0]=='9' )
                                    add_even(MOTION_T,zone);
                                else if( id[0]=='3' )
                                    add_even(DOOR_T,zone);
                                else if( id[0]=='1' )
                                    add_even(PANIC_T,zone);
                                else if( id[0]=='B' )
                                    add_even(HVAC_T,zone);
                                else if( id[0]=='5' )
                                    add_even(APPLIANCE_T,zone);
                                else if( id[0]=='4' )
                                    add_even(RESERVE1_T,zone);
                                else if( id[0]=='7' )
                                    add_even(RESERVE2_T,zone);
                                else if( id[0]=='A' )
                                    add_even(RESERVE3_T,zone);
                                else if( id[0]=='D' )
                                    add_even(RESERVE4_T,zone);
                                else if( id[0]=='E' )
                                    add_even(RESERVE5_T,zone);
                                else if( id[0]=='0' )
                                    add_even(RESERVE6_T,zone);
                                else if( id[0]=='F' )
                                    add_even(RESERVE7_T,zone);
                            }
                            if( (rx2_buf[4]&0x04)!=0 )   //test
                            {
                                add_even(TEST_PIN_T,zone);                            
                            }    
                            if( ((rx2_buf[4]&0x02)!=0) )   //Tamper open
                            {
                                if( ID_LIST[zone-3][8]==0 )
                                {
                                    add_even(TAMPER_OPEN_T,zone);     
                                    ID_LIST[zone-3][8]=1; 
                                }
                            }else                           //Tamper close
                            {
                                if( ID_LIST[zone-3][8]==1 )
                                {
                                    add_even(TAMPER_CLOSE_T,zone);     
                                    ID_LIST[zone-3][8]=0; 
                                }
                            }
                            if( (rx2_buf[4]&0x10)!=0 )   //Low Battery
                            {
                                add_even(LOW_BATTERY_T,zone);                            
                            }     
                            if( (rx2_buf[4]&0x80)!=0 )   //Supervisory
                            {
                                add_even(SUPERVISORY_T,zone);                            
                            }     
                            /*out_sbuf2('$');
                            out_sbuf2('A');
                            out_sbuf2('S');
                            out_sbuf2(0x0d);
                            out_sbuf2(0x0a);*/
                        }
                    }else{
                        
                        if( learning_mode==KEY_ADD_ID&&zone==0 )
                            zone = add_ID(&id); 
                        else if( learning_mode==KEY_DEL_ID&&zone!=0 )
                            zone = del_ID(zone); 
                        
                        /*out_sbuf2('$');
                        out_sbuf2('A');
                        out_sbuf2('L');
                        out_sbuf2(0x0d);
                        out_sbuf2(0x0a);*/
                    }    
                    //send respond
                    for( rx2_cnt=0;rx2_cnt<7;rx2_cnt++)
                        out_sbuf2(rx2_buf[rx2_cnt]);
                    //------------
                }
                rx2_cnt = 0;
                CREN2 = 0;
                NOP();
                CREN2 = 1;
            }
        }while(RC2IF==1);
       // RC1IF = 0;
    }        
//================ Smoker ======================================       
    if( ver_select == SMOKER )
    {
        if( INT0IF==1 )     //STANDBY(PIN4)
        {
            INT0IF = 0;
            Standby_f = 1;
        }
        if( INT1IF==1 )     //ALARM(PIN5)
        {
            //LED = 0;
            INT1IF = 0;
            if( TEST_PIN==1 )
            {
                //if( test_enable==1||first_test!=0||Test_click==1 )
                //    add_even(TEST_PIN_T,2);
            }else alarm_count = 1;                              
        }
        if( INT2IF==1 )     //ERROR(PIN6)
        {
            INT2IF = 0;
            Error_f = 1;
        }
        if( RBIF==1 )       //TEST(PIN1)    RB.4
        {
            if( TEST_PIN==1 )            //只計算正緣觸發
            {
                test_count++;
                test_time_detect = 0;
            }
            temp = PORTB;
            NOP();
            RBIF = 0;
        }
    }
//================Super Hub======================================    
    if( ver_select == SUPER_HUB )
    {
       if( INT0IF==1 )     //Tamper SW
        {
            INT0IF = 0;
            if( tamper_status==0 )
            {
                add_even(TAMPER_OPEN_T,1);
                tamper_status = 1;        
                SPK = 0;
            }
        }
        if( INT1IF==1 )     //Learning
        {
            INT1IF = 0;
        //    if( learn_delay==0 )
       //     {
                test_count++;
                test_time_detect = 0;                             
                learn_delay = 2;
        //    }
        }
    }
//==============================================================
    if( TMR0IF==1 )
    {                
        TMR0IF = 0;        
        if( HL_freq==0 )    //31.25KHz
        {
            TMR0L = ((65536-LOW_FREQ_TMR0)%256);
            TMR0H = ((65536-LOW_FREQ_TMR0)/256);
        }else
        {
            TMR0L = ((65536-HIGH_FREQ_TMR0)%256);
            TMR0H = ((65536-HIGH_FREQ_TMR0)/256);
        }
       // SPK = ~SPK;
        if( first_test!=0 )
            first_test--;
    //    if( adc_count%2900==0 )       //for 電池電壓測試(每5分鐘)
    //        add_even(TEST_CYCLE_S,2);
        if( VER_SELECT==1&&TEST_PIN==1 )
        {
            if( ++test_9sec_count==90 )   //100ms*90
                add_even(TEST_PIN_T,0);
        }else test_9sec_count = 0;
        
        if( ++adc_count>=(36000*0.9888) )       //[3600*10]*100ms=3600sec=1hr            //0.979  -1.56
        {            
            chk_supervisory++;    //----add supervisory
            adc_count = 0;   
            if( ++Respond_T_Hour>=24 )       //24
            {
                Respond_T_Hour = 0;
                Respond_T_Day++;
                if( Respond_T_Day>= respond_day )         
                {
                    Respond_T_Day = 0;
                    add_even(TEST_CYCLE_S,0);             
                    rsp_SUP_LBT();
                    OTA_flag = 1;
                }
                if( BT_S_respond!=0 )
                    BT_S_respond--;
                if( BT_L_respond!=0 )
                    BT_L_respond--;
                if( EOL_respond!=0 )
                    EOL_respond--;
            }   
            if( OTA_flag==2 )
                OTA_flag = 1;
        }else if( (adc_count%600)==0 )        //every 10 mins check low battery
            ADC_time = 1;
               
        if( retry_count!=0 )
            retry_count--;
        
        if( led_count!=0 )
        {
            if( --led_count==0 )
            {
                LED_RX_IN = 1;
                LED_RX_OUT = 1;
            }
        }
        
        if( tamper_status!=0&&TAMPER_PIN==1  )
        {
            if( ++tamper_status>5 )
            {
                if( first_tamper==0 )                    
                    add_even(TAMPER_CLOSE_T,1);
                first_tamper = 0;
                tamper_status = 0;
                SPK = 1;
            }
        }
        
        if( learn_delay!=0 )
            learn_delay--;
        
        if( learning_mode == KEY_NONE )
        {
            if( LED_flash_type!=LED_OFF )
            {
                if( LED_flash_type==LED_REGISTER )
                {
                    LED_count++;
                    if( LED_count < 5 )                
                        LED_G = 0;     
                    else LED_G = 1;
                    if( power_status==0 )
                        LED_B = 1;
                    else LED_B = 0;
                    if( LED_count>=20 )
                        LED_count = 0;
                }else if( LED_flash_type==LED_STANDBY )
                {
                    LED_count++;
                    if( LED_count < 5 )                
                        LED_B = 0;     
                    else LED_B = 1;
                    LED_G = 1;
                    if( LED_count>=20 )
                        LED_count = 0;
                }else if( LED_flash_type==LED_INTERNET )
                {
                    LED_B = ~LED_B;
                    LED_G = 1;
                }else if( LED_flash_type==LED_NO_SET )
                {
                    LED_count++;
                    if( LED_count<5 )
                    {
                        LED_B = 0;     
                        LED_G = 1;
                    }else if( LED_count<=9 )
                    {
                        LED_B = 1;     
                        LED_G = 0;               
                    }else LED_count = 0;
                }else if( LED_flash_type==LED_NET_ERR )
                {
                    LED_count++;         
                    if( LED_count==0 )
                    {
                        LED_B = 0;     
                        LED_G = 1;
                    }else if( LED_count==1 )
                    {
                        LED_B = 1;     
                        LED_G = 1;               
                    }else if( LED_count==2 )
                    {
                        LED_B = 0;     
                        LED_G = 1;               
                    }else if( LED_count==3 )
                    {
                        LED_B = 1;     
                        LED_G = 1;               
                    }else if( LED_count==4 )
                    {
                        LED_B = 1;     
                        LED_G = 0;               
                    }else if( LED_count==5 )
                    {
                        LED_B = 1;     
                        LED_G = 1;               
                    }else if( LED_count==6 )
                    {
                        LED_B = 1;     
                        LED_G = 0;               
                    }else if( LED_count==7 )
                    {
                        LED_B = 1;     
                        LED_G = 1;               
                    }else LED_count = 0;            
                }
            }else{
                if( retry_count==0 )
                {
                    LED_B = 1;
                    LED_G = 1;
                }else       //new add V104
                {
                    LED_count++;
                    if( LED_count < 3 )                
                    {
                        LED_G = 0;     
                        LED_B = 0;
                    }
                    else
                    {
                        LED_G = 1;
                        LED_B = 1;
                    }                
                    if( LED_count>=50 )
                        LED_count = 0;
                }
            }
        }else{
            if( ++exit_learn>=600 )     //100*100ms=10Sec
                learning_mode = KEY_NONE;
            if( learning_mode==KEY_IN_LEARN)
            {
                LED_B = 0;
                LED_G = 0;
            }else if( learning_mode==KEY_ADD_ID )
            {
                LED_count++;
                if( LED_count < 5 )                
                    LED_B = 0;     
                else LED_B = 1;
                LED_G = 0;
                if( LED_count>=20 )
                    LED_count = 0;
            }
            else if( learning_mode==KEY_DEL_ID )
            {
                LED_count++;
                if( LED_count < 5 )                
                    LED_G = 0;     
                else LED_G = 1;
                LED_B = 0;
                if( LED_count>=20 )
                    LED_count = 0;          
            }
        }
        if( ver_select == SMOKER )
        {
            if( alarm_count!=0 )
            {
                if( ALARM_PIN==1 )
                {
                    if( ++alarm_count>=10 )
                    {
                        add_even(SMOKE_ALARM_T,1); 
                       // send_trigger_to_RF(1);
                        alarm_count = 0;
                    }
                }else alarm_count = 0;
            }
            if( ERROR_PIN==1 )
            {
                if( ++err_count>=10 )   //100ms*10 = 1 sec
                {
                    if( err_count==10 )
                    {
                     //   if( alarm_out(TAMPER_OPEN_T)==0 )
                      //      Tamper_open_f = 1;
                            add_even(TAMPER_OPEN_T,1);
                    }
                    err_count = 10;
                }
            }else
            {
                if( err_count>=10 )
                {
                  //  if( alarm_out(TAMPER_CLOSE_T)==0 )
                    //    Tamper_close_f = 1;
                        add_even(TAMPER_CLOSE_T,1);
                }
                err_count = 0;
            }   
        
            if( Standby_f==1 )
            {
                Standby_f = 0;
                standby_count = 0;
            }else{
                standby_count++;            //100ms*4000=400Sec
                if( standby_count>= 4000)
                {
                    standby_count = 0;
                    Smoke_respond = 1;
                }
            }
        
            if( Error_f==1 )
            {
                Error_f = 0;
                error_count++;
                error_time_detect = 0;
            }else
            {            
                if( error_count!=0 )
                {
                    if( ++error_time_detect>=300 )  //100ms*300=30sec
                    {                    
                        if( error_count==1 )            
                        {
                            if( ++error_status_count>=5 )
                            {
                                if( BT_S_respond==0 )
                                {
                                    add_even(LOW_BATTERY_T,1);
                                    error_status_count = 0;
                                    BT_S_respond = BT_EOL_RESEND;
                                }
                            }
                        }else if( error_count==3 )
                        {
                            if( ++error_status_count>=5 )
                            {
                                if( EOL_respond==0 )
                                {
                                    add_even(EOL_T,1);
                                    error_status_count = 0;
                                    EOL_respond = BT_EOL_RESEND;
                                }
                            }
                        }else error_status_count = 0;
                        error_count = 0;
                        error_time_detect = 0;                                            
                    }
                }else
                {
                    if( ++error_time_detect>=300 )  //100ms*300=30sec
                    { 
                        error_time_detect = 0;   
                        error_status_count = 0;                    
                    }
                }
            } 
        }             
        if( test_count!=0 )
        {
            test_time_detect++;
            if( learning_mode == KEY_NONE )
            {
                if( test_count==5&&test_time_detect>10 ) //1sec
                {
                   learning_mode = KEY_IN_LEARN;
                   exit_learn = 0;
                   test_count = 0;
                    test_time_detect = 0;
                }else if( test_time_detect>=20 )  //100ms*20=2sec
                {
                    test_count = 0;
                    test_time_detect = 0;
                }
            }else{                                                        
                if( ++test_time_detect>=20 )  //100ms*20=2sec
                {               
                    if( test_count==1 )
                    {
                        Test_click = 1;
                        add_even(GO_SMS_T,0);
                        learning_mode = KEY_NONE;
                    }else if( test_count==2 )
                    {
                        learning_mode = KEY_ADD_ID;                        
                    }else if( test_count==3 )
                    {
                        learning_mode = KEY_DEL_ID;                   
                    }else if( test_count==4 )
                    {
                        add_even(TEST_PIN_T,0);
                     //   send_trigger_to_RF(0);
                        learning_mode = KEY_NONE;
                    }else if( test_count==5 )
                    {
                        learning_mode = KEY_NONE;
                    }                         
                    test_count = 0;
                    test_time_detect = 0;
                }
           }       
        }
        if( RF_wait_count!=0 )
        {
            LED_G = 0;
            if( --RF_wait_count==0 )
            {           
               // OSCCON = LOW_FREQ_OSCCON;
                HL_freq = 0;
                T0CON = LOW_FREQ_T0CON;             //1*4000 = 50,000us
                TMR0IE = 0;//
                TMR0ON = 0;
            }
        }
    }
}

