#include "initial.h"
#include "EE_library.h"
#include "System_Library.h"
#include "Module_Library.h"
#include "emc_library.h"
#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include <pic18f26k22.h>
#include <xc.h>

unsigned char wait_connect_respond(unsigned int count)
{
  	unsigned char temp;
    unsigned char buffer[20],buffer_p;
    unsigned char bad[3];
    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    buffer_p = 0;
    CLRWDT();
  	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        TMR3IF = 0;
     	do{
        	if( RC1IF==1)
        	{	   
		 	 	temp=RC1REG;
                buffer[buffer_p] = temp;
                bad[0] = bad[1];
                bad[1] = bad[2];
                bad[2] = temp;
                if( ++buffer_p>=20 )
                    buffer_p = 19;
                if( temp==0x0a )
                {
                    if( buffer[0]=='C'&&buffer[1]=='O'&&buffer[2]=='N'&&buffer[3]=='N'&&buffer[4]=='E'&&buffer[5]=='C'&&buffer[6]=='T' )
                        temp = 'C';
                    else if( buffer[0]=='N'&&buffer[1]=='O'&&buffer[2]==' '&&buffer[3]=='C'&&buffer[4]=='A'&&buffer[5]=='R'&&buffer[6]=='R' )
                        temp = 'E';  
                    else if( buffer[0]=='E'&&buffer[1]=='R'&&buffer[2]=='R'&&buffer[3]=='O'&&buffer[4]=='R' )
                        temp = 'E';  
                    if(temp=='C'||temp=='E')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        //delay5ms(60);
                        return(temp);		    		
                    }
                    buffer_p = 0;
                }
                if( bad[0]=='B'&&bad[1]=='A'&&bad[2]=='D' )
                {
                    RC1IE = 1;
                    CREN1 = 0;
                    return('B');	
                }
        	}
            if( OERR1==1 )
            {
                CREN1 = 0;
                CREN1 = 1;
            }
     	}while(TMR3IF==0);
        CLRWDT();
        TMR3IF = 0;
        //LED = ~LED;
  	}while(--count!=0);
  	TMR3ON = 0;
    RC1IE = 1;
  	return('N');
}

unsigned char CRC_16(unsigned char tp_cnt)
{

    unsigned char cnt,loop; 
    unsigned int reg_crc=0xFFFF; 
    for( cnt=0;cnt<tp_cnt;cnt++) 
    { 
        reg_crc ^= rsp_buffer[cnt]; 
        for(loop=0;loop<8;loop++) 
        { 
            if(reg_crc & 0x01)
                reg_crc=(reg_crc>>1) ^ 0xA001; 
            else 
                reg_crc=reg_crc >>1; 
        } 
    } 
    rsp_buffer[tp_cnt++] = reg_crc&0x00ff;
    rsp_buffer[tp_cnt++] = reg_crc>>8;
    return tp_cnt; 
}

unsigned char decryption_data(unsigned char buffer_p,unsigned char *buffer )
{
   	unsigned char cnt,temp1,temp2,random,swap;
    /*---decrypt code---*/
	random = buffer[0];
	random = ((random>>4)+(random&0x0f))%16;
	//random = 0;
	for( cnt=1;cnt<(buffer_p-1);cnt++ )
	{
		temp2 = buffer[cnt];
		temp1 = encryption_code[random];	
		temp2 ^= temp1;					//xor		
		swap = temp2 <<6;				//left shift 2 bit
		temp2 >>= 2;
		temp2 += swap;
		swap = temp2<<4;					//4bit swap
		temp2 >>=4;
		temp2 += swap;	
		temp2 ^= temp1;					//xor	
		buffer[cnt-1] = temp2;
		if( ++random>=16 )
			random = 0;			
	}
	return(cnt-1);       
}

unsigned char encryption_data(unsigned char tp_cnt )
{
    unsigned char temp,cnt1,cnt2,temp1,random,swap,type,page;
	unsigned char check_sum,random_log;
	/*------------random number---------------*/
	random = (rand()>>8)^rand()^random_rx;
	random_log = random;
	check_sum = random;
	random = ((random>>4)+(random&0x0f))%16;	
	/*------------encryption ---------------*/
    cnt1 = 0;
	do{
		temp = rsp_buffer[cnt1];
		/*------------encryption ---------------*/
		temp1 = encryption_code[random];		
		temp ^= temp1;					//xor
		swap = temp<<4;					//4bit swap
		temp >>=4;
		temp += swap;	
		swap = temp >>6;				//left shift 2 bit
		temp <<= 2;
		temp += swap;
		temp ^= temp1;					//xor
		rsp_buffer[cnt1] = temp;
		if( ++random>=16 )
			random = 0;
		/*-------------encryption --------------*/
		check_sum ^= temp;	  	
	}while(++cnt1<tp_cnt);
    CLRWDT();
    for( cnt1=tp_cnt;cnt1--;cnt1!=0 )
    {
        rsp_buffer[cnt1] = rsp_buffer[cnt1-1];        
    }
    rsp_buffer[0] = random_log;
    tp_cnt++;
	rsp_buffer[tp_cnt++] = check_sum;
    return(tp_cnt);
}

unsigned char OTA_send_data_to_server(void)
{
    unsigned char const send[]="AT#SSENDEXT=1,$";
    unsigned char buffer_p;
	unsigned char cnt,temp,as;
    unsigned char tp_cnt;
    
    // NEJ2040!< 
    tp_cnt = 0;
    rsp_buffer[tp_cnt++] = 'N';
    rsp_buffer[tp_cnt++] = 'E';
    rsp_buffer[tp_cnt++] = 'J';
    rsp_buffer[tp_cnt++] = VERSION[0];
    rsp_buffer[tp_cnt++] = VERSION[1];
    rsp_buffer[tp_cnt++] = VERSION[2];
    rsp_buffer[tp_cnt++] = VERSION[3];
    // CRC
    tp_cnt = CRC_16(tp_cnt);
    //encryption
    tp_cnt = encryption_data(tp_cnt);
    
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

unsigned char OTA_receive_data_from_server(void)
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
                        buffer_p = decryption_data(buffer_p,&buffer[0]);
                        if( buffer[0]=='N'&&buffer[1]=='E'&&buffer[2]=='J'&&buffer[3]==VERSION[0]&&buffer[4]==VERSION[1]&&buffer[5]==VERSION[2]&&buffer[6]==VERSION[3] )
                        {
                            if( buffer[9]==0x0d )
                                temp = 'K';
                            else if( buffer[7]=='R'&&buffer[8]=='F'&&buffer[9]=='Q' )                            
                                temp = 'U';                            
                            else temp = 'R';
                        }else temp = 'E';
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

//---------------------------------------------------
unsigned char OTA_connection_open(unsigned char type)   //0: command mode 1:online mode
{
	unsigned char const netconnect[]="AT#SD=1,0,$";
    unsigned char const net_2[]="\",0,0,1\r\n$"; 
    unsigned char const net_3[]="\",0,0,0\r\n$";
    //unsigned char const netconnect[]="AT#SD=1,0,2020,\"211.22.241.58\",0,0,1\r\n$";
    unsigned char buffer_p,buffer[32];
	unsigned char cnt,temp;
	unsigned int port;
    unsigned char ip_sel,page,addr;
    temp = read_ee(0x01,0xd0);
    if( temp=='#')
    {
        temp = read_ee(0x00,0x30);        
        ip_sel = 1;
    }else ip_sel = 0;
    if( temp=='#' )
        return('E');
    CREN1 = 0;
	soutdata(&netconnect);
    //------ port ------
    if( ip_sel == 0)
    {
        page = 1;
        addr = 0xf0;
    }else 
    {
        page = 0;
        addr = 0xB0;
    }

	port = read_ee(page,addr)<<8;
	port += read_ee(page,addr+1);
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
    if( ip_sel == 0)
    {
        page = 1;
        addr = 0xd0;
    }else 
    {
        page = 0;
        addr = 0x30;
    }
	do{
		temp = read_ee(page,addr);
		if( temp!='#' )
			out_sbuf(temp);
		addr++;
	}while( temp!='#' );     
    CLRWDT();
    if( type==0 )        
        soutdata(&net_2);
    else soutdata(&net_3);
	//Connect ok / fail
	buffer_p = 0;
	cnt = 200;
    RCIE = 0;
	CREN1 = 1;
    if( type==0 )
        temp = wait_ok_respond(500);
    else temp = wait_connect_respond(1500);
    return(temp);
}

unsigned char Check_OTA(void)
{
    unsigned char rsp,cnt,cnt1;
    cnt = 3;
    do{
        rsp = TL_internet_init();
        if( rsp=='K' )
        {
            rsp = OTA_connection_open(0x00);
            if( rsp=='K' )
            {
                rsp = OTA_send_data_to_server();
                cnt1 = 50;
                do{
                    rsp = OTA_receive_data_from_server();     
                    delay5ms(200);
                }while(--cnt1!=0&&rsp=='E');
                //online mode
                if( rsp=='U' )
                {
                    delay5ms(200);
                    TL_connection_close();
                    delay5ms(200);
                    rsp = OTA_connection_open(0x01); 
                    if( rsp=='C' )
                    {
                        out_sbuf('R');
                        out_sbuf('F');
                        out_sbuf('Q');
                        rsp = wait_connect_respond(6000);
                        out_sbuf('+');
                        out_sbuf('+');
                        out_sbuf('+');
                        delay5ms(200);
                        delay5ms(200);
                        delay5ms(200);
                        if( rsp=='B' )
                        {                        
                            TL_connection_close();
                            delay5ms(200);
                            TL_internet_close();
                            return('K');
                        }                          
                    }
                }else if( rsp=='K' )
                {
                    TL_connection_close();
                    delay5ms(200);
                    TL_internet_close();
                    return('K');
                }else if( rsp=='R' )
                {                    
                    TL_connection_close();
                    delay5ms(200);
                    TL_internet_close();
                    return('E');
                }
            }
            TL_connection_close();
        }
        delay5ms(200);
        TL_internet_close();
        delay5ms(200);
    }while(--cnt!=0);
    return('E');
}
