#include "initial.h"
#include "EE_library.h"
#include "System_Library.h"
#include "Module_Library.h"
#include "emc_library.h"
#include <pic18f26k22.h>
#include <xc.h>

void GM_module_first_run(void)
{    
    unsigned char const led[]="AT^SCFG=\"GPIO/mode/SYNC\",\"std\"\r\n$";
    unsigned char const led1[]="AT^SLED=2\r\n$";      
    unsigned char const led2[]="AT^SLED=2,50\r\n$"; 
    unsigned char const mdm[]="AT^SCFG=\"URC/DstIfc\",\"mdm\"\r\n$";

    //led
    soutdata("AT+CFUN=1\r\n$");
    wait_ok_respond(20); 
    soutdata(&led);
    wait_ok_respond(40);
    if(  Module_type == EMS31||Module_type == PLS8 )
        soutdata(&led1);
    else soutdata(&led2);
    wait_ok_respond(40); 
    //time   
    soutdata("AT^SPOW=1,0,0\r\n$");
    wait_ok_respond(40);
    soutdata(&mdm);
    wait_ok_respond(40);
}

//---------------------------------------------------
unsigned char GM_internet_init(void)
{
	unsigned char const cgatt[]="AT+CGATT=1\r\n$";
    unsigned char const gprs0[]="AT^SICS=0,contype,gprs0\r\n$";
    unsigned char const gprs1[]="AT^SICS=0,apn,$";
    unsigned char const gprs3[]="AT^SISS=1,srvType,Socket\r\n$";
    unsigned char const gprs4[]="AT^SISS=1,conId,0\r\n$";
	
	unsigned char cnt,temp;

    CREN1 = 0;
    delay5ms(100);
	soutdata(cgatt);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);
    CREN1 = 0;
   	delay5ms(100);
	soutdata(gprs0);
//	if( wait_ok_respond(200)=='N' )
        wait_ok_respond(200);
    delay5ms(100);
    soutdata(gprs1);
	cnt = 0x10;
	do{
		temp = read_ee(0x00,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );
	out_sbuf(0x0d);
	out_sbuf(0x0a);
//	if( wait_ok_respond(200)=='N' )
		wait_ok_respond(200);   	
    
    CREN1 = 0;
   	delay5ms(100);
	soutdata(gprs3);
    
    CREN1 = 0;
   	delay5ms(100);
	soutdata(gprs4);
    delay5ms(100);
    
    return('K');
}

//---------------------------------------------------
unsigned char GM_internet_init_EMS31(void)
{
    unsigned char const gprs3[]="AT^SISS=1,srvType,Socket\r\n$";
    unsigned char const gprs4[]="AT^SISS=1,conId,1\r\n$";
    
	unsigned char const cgatt[]="AT+CGATT=1\r\n$";
	unsigned char const cgdcont[]="AT+CGDCONT=1,\"IP\",\"$";	
    //unsigned char const cgdcont[]="AT+CGDCONT=1,\"IP\",\"internet\"\r\n$";	
    unsigned char const sgact[]="AT^SICA=1,1\r\n$";    
    unsigned char const cgpaddr[]="AT+CGPADDR=1\r\n$";
	unsigned char cnt,temp,count;
    unsigned char buffer_p,buffer[32];

    CREN1 = 0;
   // delay5ms(100);
	//soutdata(scfg);
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
					if( buffer[0]=='O'&&buffer[1]=='K' )
                    {
                        delay5ms(100);
                        soutdata(cgpaddr);                        
                        delay5ms(100);
                        soutdata(gprs3);
                         delay5ms(100);
                        soutdata(gprs4);
                        delay5ms(100);
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        TMR3IF = 0;
  	}while(--count!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
unsigned char GM_connection_open(unsigned char type)
{
    //at^siss=1,address,"socktcp://sohu.com:80"
	unsigned char const netconnect[]="AT^SISS=1,address,\"socktcp://$";  
    unsigned char const net_2[]="\"\r\n$";
    unsigned char const open[]="AT^SISO=1\r\n$";
    unsigned char buffer_p,buffer[32];
	unsigned char cnt,temp,count;
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
    //------ IP ------    
    if( type==0x01 )
		cnt = 0x30;
	else if( type==0x02 )
		cnt = 0x50;
	else if( type==0x03 )
		cnt = 0x70;
	else cnt = 0x90;
	do{
		temp = read_ee(0x00,cnt);
		if( temp!='#' )
			out_sbuf(temp);
		cnt++;
	}while( temp!='#' );    
    out_sbuf(':');
    //------ port ------
    if( type==0x01 )
		cnt = 0xB0;
	else if( type==0x02 )
		cnt = 0xB2;
	else if( type==0x03 )
		cnt = 0xB4;
	else cnt = 0xB6;

	port = read_ee(0x00,cnt)<<8;
	port += read_ee(0x00,cnt+1);
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
   //-----------------------------    
	soutdata(&net_2);
    wait_ok_respond(200);
    
	//Connect ok / fail
    CREN1 = 0;
    soutdata(&open);
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
		  		if( temp==0x0a )	//Network opened    ^SISW: 1, 1
				{
					if( buffer[0]=='^'&&buffer[1]=='S'&&buffer[2]=='I'&&buffer[3]=='S'&&buffer[4]=='W'&&buffer[5]==':' )
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        return('K');
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        TMR3IF = 0;
  	}while(--count!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
	delay5ms(100);
    return('E');
}

//---------------------------------------------------
void GM_connection_close(void)
{
	unsigned char const TCP_close[]="AT^SISC=1\r\n$";

    CREN1 = 0;
	delay5ms(200);
	soutdata(&TCP_close);
	delay5ms(200);
}

//---------------------------------------------------
void GM_internet_close(void)
{
	//unsigned char const NT_close[]="AT#SGACT=1,0\r\n$";

    CREN1 = 0;
	delay5ms(200);
	//soutdata(&NT_close);
	//delay5ms(200);
}

unsigned char GM_send_data_to_server(void)
{
    unsigned char const send[]="AT^SISW=1,$";
    unsigned char buffer_p;
	unsigned char cnt,temp,as;
    unsigned char tp_cnt,buffer_p,buffer[100];
    
    tp_cnt = enc_cnt;
    if( encryption==0 )
        tp_cnt--;
    CREN1 = 0;
    soutdata(&send);
    cnt = tp_cnt;
    if( tp_cnt>100 )
        out_sbuf((cnt/100)+0x30);
    cnt %= 100;
    if( tp_cnt>10 )
        out_sbuf((cnt/10)+0x30);
    out_sbuf((cnt%10)+0x30);
    out_sbuf('\r');
    out_sbuf('\n');
    RCIE = 0;
    CREN1 = 1;
    as = 100;
    buffer_p = 0;
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
					if( buffer[0]=='^'&&buffer[1]=='S'&&buffer[2]=='I'&&buffer[3]=='S'&&buffer[4]=='W'&&buffer[5]==':' )
                    {
                        TMR3ON = 0;
                        CREN1 = 0;
                        as = 1;
                        break;
                    }
					buffer_p = 0;
			 	}
        	}
     	}while(TMR3IF==0);
        TMR3IF = 0;
  	}while(--as!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    CREN1 = 0;
    delay5ms(100);
    cnt = 0;
    do{
        temp = rsp_buffer[cnt];
        out_sbuf(temp);
    }while(++cnt<tp_cnt);
    RCIE = 0;
    CREN1 = 1;
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
	temp = wait_ok_respond(200);
    return(temp);
}

unsigned char GM_receive_data_from_server(void)
{
//      AT#SRECV=1,20
//      #SRECV: 1,9
//      +++123456
//
//      OK
    unsigned char const SRECV[]="AT^SISR=1,200\r\n$";
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
                        if( buffer[0]=='^'&&buffer[1]=='S'&&buffer[2]=='I'&&buffer[3]=='S'&&buffer[4]=='R'&&buffer[5]==':'&&
                            buffer[6]==' '&&buffer[7]=='1'&&buffer[8]==','   )
                            if( buffer[9]!='0' )
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
        TMR3IF = 0;
  	}while(--cnt!=0);
    TMR3IF = 0;
  	TMR3ON = 0;
    return('E');
}

