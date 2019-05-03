#include "initial.h"
#include "EE_library.h"
#include "Module_Library.h"
#include "emc_library.h"
#include <pic18f26k22.h>
#include <xc.h>

void loed_ID_to_buffer(void);

void delay5ms(unsigned int cnt)
{
    unsigned char a,b;
    if( HL_freq==1 )
    {
        do{
            b = 18;
            do{
                a = 50;//100;
                while(--a!=0);
            }while(--b!=0);
            CLRWDT();
        }while(--cnt!=0);
    }else
    {
        do{
            a = 4;//100;
            while(--a!=0); 
            CLRWDT();
        }while(--cnt!=0);
    }
}

void first_run(void)
{
#ifdef DEBUG
    unsigned char const APN[]="internet#";            //35#
    //unsigned char const IP1[]="198.17.112.128#";       //01#
    unsigned char const IP1[]="106.104.30.120#";
#else
    unsigned char const APN[]="#";                    //35#
    unsigned char const IP1[]="#";                    //01#
#endif
#ifdef DEBUG
    unsigned char const IP2[]="106.104.30.120#";                    //02#
    unsigned char const IP3[]="106.104.30.120#";                    //03#
    unsigned char const IP4[]="106.104.30.120#";                    //04#
    unsigned char const IP_OTA[]="106.104.30.120#";                  //36#
    unsigned int const PORT1 = 2020;                   //31#   
    unsigned int const PORT2 = 2020;                   //32#   
    unsigned int const PORT3 = 2020;                   //33#   
    unsigned int const PORT4 = 2020;                   //34#   
    unsigned int const PORT_OTA = 3030;                   //37# 
#else
    unsigned char const IP2[]="#";                    //02#
    unsigned char const IP3[]="#";                    //03#
    unsigned char const IP4[]="#";                    //04#
    unsigned char const IP_OTA[]="#";                  //36#
    unsigned int const PORT1 = 2020;                   //31#   
    unsigned int const PORT2 = 2020;                   //32#   
    unsigned int const PORT3 = 2020;                   //33#   
    unsigned int const PORT4 = 2020;                   //34#   
    unsigned int const PORT_OTA = 2020;                   //37#   
#endif
    unsigned char const ACCESS_CODE[]="1111#";        //05#
    unsigned char const PROGRAM_ACK=0x01;              //06#
    unsigned char const TEST_FREQ=15;                  //07#
    unsigned char const SERVER_ACK_TIME=45;            //08#
    unsigned char const UNIT_ACCNT[]="7548#";         //10#
    unsigned char const LINE_CARD[]="4008#";             //11#
    unsigned char const ZONE1='0';                  //12#
 //   unsigned char const ZONE2=20;                  //13#
    unsigned char const TP_PIN=0;                  //14#
    unsigned char const CYCLE = 3;                  //15#
    unsigned char const RETRY = 30;                  //16#
#ifndef DEBUG
    unsigned char const SMS_WAIT_TIME = 10;             //09#
#else
    unsigned char const SMS_WAIT_TIME = 3;             //09#
#endif
    unsigned char const ENCRYPTION = 1;                 //95#
   
    unsigned char temp,cnt;
    unsigned int int_d;
    unsigned char page,addr;
    //---------Check Version-----------
    if( read_ee(0x00,0x00)==VERSION[0] )
    {
        if( read_ee(0x00,0x01)==VERSION[1] )
        {
            if( read_ee(0x00,0x02)==VERSION[2] )
                return;
        }       
    }
    //---------APN-----------
    page = 0x00;
    addr = 0x10;
    cnt = 0;
    do{
        temp = APN[cnt];
        write_ee(page,addr+cnt,temp);
    }while(++cnt<31&&temp!='#');
    //---------IP1-----------
    page = 0x00;
    addr = 0x30;
    cnt = 0;
    do{
        temp = IP1[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------IP2-----------
    page = 0x00;
    addr = 0x50;
    cnt = 0;
    do{
        temp = IP2[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------IP3-----------
    page = 0x00;
    addr = 0x70;
    cnt = 0;
    do{
        temp = IP3[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------IP4-----------
    page = 0x00;
    addr = 0x90;
    cnt = 0;
    do{
        temp = IP4[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------PORT1---------
    page = 0x00;
    addr = 0xB0;
    int_d = PORT1;
    write_ee(page,addr,(int_d>>8));
    write_ee(page,(addr+1),(int_d&0x00ff));
    //---------PORT2---------
    page = 0x00;
    addr = 0xB2;
    int_d = PORT2;
    write_ee(page,addr,(int_d>>8));
    write_ee(page,(addr+1),(int_d&0x00ff));
    //---------PORT3---------
    page = 0x00;
    addr = 0xB4;
    int_d = PORT3;
    write_ee(page,addr,(int_d>>8));
    write_ee(page,(addr+1),(int_d&0x00ff));
    //---------PORT4---------
    page = 0x00;
    addr = 0xB6;
    int_d = PORT4;
    write_ee(page,addr,(int_d>>8));
    write_ee(page,(addr+1),(int_d&0x00ff));
    //---------ACCESS_CODE-----------
    page = 0x00;
    addr = 0xC0;
    cnt = 0;
    do{
        temp = ACCESS_CODE[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------PROGRAM_ACK-----------
    write_ee(0x00,0xC7,PROGRAM_ACK);                //06#
    //---------TEST_FREQ-----------
    write_ee(0x00,0xC8,TEST_FREQ);                  //07#
    //---------SERVER_ACK_TIME-----------
    write_ee(0x00,0xC9,SERVER_ACK_TIME);            //08#
    //---------SMS_WAIT_TIME-----------
    write_ee(0x00,0xB8,SMS_WAIT_TIME);              //09#
    //---------UNIT_ACCNT-----------
    page = 0x00;                                    //10#
    addr = 0xCA;
    cnt = 0;
    do{
        temp = UNIT_ACCNT[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------LINE_CARD-----------                 
    page = 0x00;                                    //11#
    addr = 0xD0;
    cnt = 0;
    do{
        temp = LINE_CARD[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');    
    //-----------ZONE---------------                
    write_ee(0,0xB9,ZONE1);                         //12#
 //   write_ee(0,0xBA,ZONE2);                          //13#
    write_ee(0x00,0xBB,TP_PIN);                      //14#
    write_ee(0x00,0xBC,CYCLE);                      //15#
    write_ee(0x00,0xBD,RETRY);                      //16# 
    //---------ENCRYPTION-----------
    write_ee(0x00,0xE0,ENCRYPTION);                 //95#
        
    write_ee(0x00,0x0f,0x00);   //MM
    
    //41~56 ID
    for( cnt=0;cnt<(16*8);cnt++ )      //28
        write_ee(1,cnt,0);    
    loed_ID_to_buffer();
    
    //---------IP OTA---------      //36
    page = 0x01;
    addr = 0xD0;
    cnt = 0;
    do{
        temp = IP_OTA[cnt];
        write_ee(page,addr+cnt,temp);
        cnt++;
    }while(temp!='#');
    //---------PORT OTA---------    //37
    page = 0x01;
    addr = 0xF0;
    int_d = PORT_OTA;
    write_ee(page,addr,(int_d>>8));
    write_ee(page,(addr+1),(int_d&0x00ff));
    
    //------------------------------
    write_ee(0x00,0x00,VERSION[0]);
    write_ee(0x00,0x01,VERSION[1]);
    write_ee(0x00,0x02,VERSION[2]);
}

void load_default(void)
{
    encryption = read_ee(0x00,0xE0);
    respond_day = read_ee(0x00,0xC8);
    test_enable = read_ee(0x00,0xBB);
}


void check_led_type(void)
{
    if( read_ee(0x00,0x10)=='#'||read_ee(0x00,0x30)=='#')    
    {
        LED_flash_type = LED_NO_SET;
        IP_type = 0;
    }else
    {
        LED_flash_type = LED_STANDBY;
        IP_type = 1;
    }
}

//---------------new add 2017/11/23
void loed_ID_to_buffer(void)
{
    unsigned char cnt1,cnt2,addr;
    for( cnt1=0;cnt1<28;cnt1++ )
    {
        addr =cnt1*8;
        for( cnt2=0;cnt2<8;cnt2++ )
            ID_LIST[cnt1][cnt2] = read_ee(1,addr+cnt2 );
        ID_LIST[cnt1][cnt2] = 0;
    }
    CLRWDT();
}

unsigned char check_ID(unsigned char *ptr)
{
    unsigned char cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ ) //28
    {
        for( cnt2=0;cnt2<6;cnt2++ )
        {
            temp = ID_LIST[cnt1][cnt2];
            if( temp != ptr[cnt2] )
            {
         //       out_sbuf2(ptr[cnt2]);
                break;             
            }
            
        }
      //  out_sbuf2(',');
        if( cnt2==6 )
        {
    //        out_sbuf2('S');
            return(cnt1+3);   //respond Zone number 3~30
        }
    }
    CLRWDT();
  //  out_sbuf2('N');
    return(0);   
}

unsigned char add_ID(unsigned char *ptr)
{
    unsigned char cnt1,cnt2,temp,addr;
    GIE = 0;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        temp = ID_LIST[cnt1][0];
        if( temp==0x00 )
        {            
            addr =cnt1*8;
            for( cnt2=0;cnt2<6;cnt2++)
            {
                temp = ptr[cnt2];
                ID_LIST[cnt1][cnt2]=temp;
                write_ee(1,(addr+cnt2),temp);
            }
            ID_LIST[cnt1][6]=0;
            write_ee(1,addr+6,0);
            ID_LIST[cnt1][7]=0;
            write_ee(1,addr+7,0);
            ID_LIST[cnt1][8]=0;
            GIE = 1;
            return('K');
        }             
    }
    CLRWDT();
    GIE = 1;
    return('E');
}

unsigned char del_ID(unsigned char id)
{
    unsigned char addr,cnt1;
    id -=3;
    addr =id*8;
    for( cnt1=0;cnt1<8;cnt1++ )
    {
        ID_LIST[id][cnt1] = 0x00;
        write_ee(1,addr+cnt1,0x00);
        return('K');
    }
    CLRWDT();
    return('E');
}

unsigned char check_supervisory(void)
{
    unsigned char cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( ID_LIST[cnt1][0]!=0 )
        {
            if( ID_LIST[cnt1][7] < 84 )         // 12 = 1Day  84 = 7Day
            {
                if( ++ID_LIST[cnt1][7] >= 84 )
                {            
                    add_even(SUPERVISORY_T,cnt1+3);
                    ID_LIST[cnt1][7] = 'S';
                }
            }
        }
        CLRWDT();
    }    
    return(0);   
}

void rsp_SUP_LBT(void)
{
    unsigned char cnt1,cnt2,temp;
    for( cnt1=0;cnt1<16;cnt1++ )    //28
    {
        if( ID_LIST[cnt1][0]!=0 )
        {
            if( ID_LIST[cnt1][6]=='B' )                  
                add_even(LOW_BATTERY_T,cnt1+3);       
            if( ID_LIST[cnt1][7]=='S' )                  
                add_even(SUPERVISORY_T,cnt1+3);            
        }
    }
    CLRWDT();
}

/*unsigned char send_trigger_to_RF(unsigned char type)
{
    if( HL_freq==0 )
    {
        RF_wait_count = 10;
        OSCCON = HIGH_FREQ_OSCCON;	// 4MHz
        T0CON = HIGH_FREQ_T0CON;             //1*4000 = 50,000us
        HL_freq = 1;
        Uart_initial_BD2();
        CREN1 = 0;          
        CREN1 = 1;
        RC2IE = 1;
    }
    WIFI_INT1 = 0;
    WIFI_INT1_TRIS = OUTPUT;
    WIFI_INT1 = 1;
    delay5ms(200);   
    WIFI_INT1 = 0;
    out_sbuf2('$');
    if( type==0 )
        out_sbuf2('T');
    else out_sbuf2('A');
    out_sbuf2(0x0d);
    out_sbuf2(0x0a);
    WIFI_INT1_TRIS = INPUT;
    WIFI_INT1 = 1;
}*/
