#include "initial.h"
#include "EE_library.h"
#include "System_Library.h"
#include "Module_Library.h"
#include "emc_library.h"
#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include <pic18f26k22.h>
#include <xc.h>

void TL_module_first_run(void)
{    
    unsigned char const led_910[] = "AT#GPIO=1,0,2\r\n$";   //910
    unsigned char const led_866[] = "AT#GPIO=7,0,2\r\n$";   //866

	unsigned char const led1[] = "AT#SLED=4\r\n$";

    //led
    soutdata("ATE1\r\n$");
    wait_ok_respond(20);
    if( Module_type==LE910)
        soutdata(&led_910);
    else soutdata(&led_866);
    wait_ok_respond(40);
    soutdata(&led1);
    wait_ok_respond(40);    
    //time
}

//---------------------------------------------------
unsigned char TL_internet_init(void)
{
    unsigned char const scfg[]="AT#SCFG=1,3,300,90,200,50\r\n$";
	unsigned char const cgatt[]="AT+CGATT=1\r\n$";
	unsigned char const cgdcont[]="AT+CGDCONT=3,\"IP\",\"$";	
    //unsigned char const cgdcont[]="AT+CGDCONT=1,\"IP\",\"internet\"\r\n$";	
    unsigned char const sgact[]="AT#SGACT=3,1\r\n$";    
	unsigned char cnt,temp,count;
    unsigned char buffer_p,buffer[32];

    CREN1 = 0;
    delay5ms(100);
	soutdata(scfg);
	delay5ms(100);
	soutdata(cgatt);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
    CREN1 = 0;
   	delay5ms(100);
	soutdata(cgdcont);
	cnt = 0x10;
	do{
		temp = read_ee(0x00,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );
	out_sbuf('"');
	out_sbuf(0x0d);
	out_sbuf(0x0a);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
   	delay5ms(100);
    
    CREN1 = 0;
	soutdata(&sgact);
	count = 0;
    buffer_p = 0;
    RCIE = 0;
    CREN1 = 1;
	do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
				buffer[buffer_p] = temp;
				if( ++buffer_p>=32 )				
					buffer_p = 31;			
		  		if( temp==0x0a )	//Network opened
				{
					if( buffer[0]=='#'&&buffer[1]=='S'&&buffer[2]=='G'&&buffer[3]=='A'&&buffer[4]=='C'&&buffer[5]=='T'&&buffer[6]==':' )
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
  	}while(--count!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
unsigned char TL_connection_open(unsigned char type)
{
	unsigned char const netconnect[]="AT#SD=1,0,$";
    unsigned char const net_2[]="\",0,0,1\r\n$";
    //unsigned char const netconnect[]="AT#SD=1,0,2020,\"211.22.241.58\",0,0,1\r\n$";
    unsigned char buffer_p,buffer[32];
	unsigned char cnt,temp;
	unsigned int port;
    if( type==0x01 )
        temp = read_ee(0x00,0x30);
    else if( type==0x02 )
        temp = read_ee(0x00,0x50);
    else if( type==0x03 )
        temp = read_ee(0x00,0x70);
    else temp = read_ee(0x00,0x90);
    if( temp=='#' )
        return('E');
    CREN1 = 0;
	soutdata(&netconnect);
    //------ port ------
    if( type==0x01 )
		cnt = 0xB0;
	else if( type==0x02 )
		cnt = 0xB2;
	else if( type==0x03 )
		cnt = 0xB4;
	else cnt = 0xB6;

	port = read_ee(0,cnt)<<8;
	port += read_ee(0,cnt+1);
	cnt = 0;
	temp = port/10000;
	if( temp!=0 )
	{	
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 10000;
	temp = port/1000;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 1000;
	temp = port/100;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	port %= 100;
	temp = port/10;
	if( temp!=0||cnt==1 )
	{
		cnt = 1;
		out_sbuf( temp+0x30 );
	}
	temp = port%10;
	out_sbuf( temp+0x30 );
    out_sbuf(',');
    out_sbuf('"');
    //------ IP ------    
    if( type==0x01 )
		cnt = 0x30;
	else if( type==0x02 )
		cnt = 0x50;
	else if( type==0x03 )
		cnt = 0x70;
	else cnt = 0x90;
	do{
		temp = read_ee(0,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );     
    CLRWDT();
	soutdata(&net_2);
	out_sbuf(0x0d);
	out_sbuf(0x0a);

	//Connect ok / fail
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
	temp = wait_ok_respond(500);
    return(temp);
}

//---------------------------------------------------
void TL_connection_close(void)
{
	unsigned char const TCP_close[]="AT#SH=1\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(&TCP_close);
	delay5ms(200);
}

//---------------------------------------------------
void TL_internet_close(void)
{
	unsigned char const NT_close[]="AT#SGACT=3,0\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(&NT_close);
	delay5ms(200);
}

unsigned char TL_send_data_to_server(void)
{
    unsigned char const send[]="AT#SSENDEXT=1,$";
    unsigned char buffer_p;
	unsigned char cnt,temp,as;
    unsigned char tp_cnt;
    
    tp_cnt = enc_cnt;
    if( encryption==0 )
        tp_cnt--;
    CREN1 = 0;
    soutdata(&send);
    cnt = tp_cnt;
    out_sbuf((cnt/100)+0x30);
    cnt %= 100;
    out_sbuf((cnt/10)+0x30);
    out_sbuf((cnt%10)+0x30);
    out_sbuf('\r');
    //out_sbuf('\n');
    RCIE = 0;
    CREN1 = 1;
    as = 200;
    do{ 
        cnt = 0;
        do{
            buffer_p = 0;
            do{
                if( RC1IF==1 )			              
                	temp = RC1REG;
            }while(++buffer_p!=0&&temp!='>');
            CLRWDT();
        }while(++cnt!=0&&temp!='>');     
    }while(++as!=0&&temp!='>');  
    CREN1 = 0;
    delay5ms(100);
   // soutdata(&ass);
    cnt = 0;
    do{
        temp = rsp_buffer[cnt];
        out_sbuf(temp);
    }while(++cnt<tp_cnt);
    CLRWDT();
    RCIE = 0;
    CREN1 = 1;
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
	temp = wait_ok_respond(200);        //200*50ms=10sec
    return(temp);
}

unsigned char TL_receive_data_from_server(void)
{
//      AT#SRECV=1,20
//      #SRECV: 1,9
//      +++123456
//
//      OK
    unsigned char const SRECV[]="AT#SRECV=1,200\r\n$";
    unsigned char buffer_p,buffer[250];
	unsigned char cnt,temp,type,err;
    CREN1 = 0;
    soutdata(&SRECV);
    RCIE = 0;
    CREN1 = 1;
    cnt = 100;
    type = 0;
    buffer_p = 0;
    debug_p = 0;
    err = 0;
    //GIE = 0;
    //TMR0IE = 0;
     do{ 
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{                  
				temp = RC1REG;
				buffer[buffer_p] = temp;
                #ifdef DEBUG
                debug_buffer[debug_p++] = temp;
                #endif
				if( ++buffer_p>=250 )				
                 buffer_p = 249;			
		  		if( temp==0x0a&&buffer[buffer_p-2]==0x0d )	//Network opened
				{
                    if( buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R'&&buffer[3]=='O'&&buffer[4]=='R' )                     
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('E');
                    }
                    if( type==0 )
                    {
                        if( buffer[0]=='#'&&buffer[1]=='S'&&buffer[2]=='R'&&buffer[3]=='E'&&buffer[4]=='C'&&buffer[5]=='V'&&buffer[6]==':' )
                            type = 1;
                    }else
                    {
                        temp = chk_data_type(&buffer,buffer_p);
                        if( temp=='E' )
                            temp = 'R';
                        return(temp);
                    }
                    buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
  	}while(--cnt!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    return('E');
}
