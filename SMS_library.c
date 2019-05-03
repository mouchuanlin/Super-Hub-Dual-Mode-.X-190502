#include <pic18f26k22.h>
#include <string.h>
#include <xc.h>
#include <ctype.h>
#include "initial.h"
#include "System_Library.h"
#include "Module_Library.h"
#include "EE_library.h"
#include "SMS_library.h"
#include "Setup_mode.h"

unsigned char sms_p,sms_buffer[160]; 
unsigned char time[9];
unsigned char sms_phone[40];
unsigned char phone_cnt;
unsigned char access_code[7];
unsigned char x_cnt;

//---------------------------------------------------

void set_sms_init(void)
{
    unsigned char const cmgf[] = "AT+CMGF=1\r\n$";
    unsigned char const cpms[]="AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n$";
	unsigned char const cnmi[]="AT+CNMI=1,1\r\n$";
    //sms
    CREN1 = 0;
    soutdata(&cmgf);
    wait_ok_respond(40);
 //   soutdata(&cpms);
 //   wait_ok_respond(40);
 //   soutdata(&cnmi);
 //   wait_ok_respond(40);    
}

//---------------------------------------------------
unsigned char check_sms(void)
{
  	unsigned char const cmgl[]="AT+CMGL=\"ALL\"\r\n$";
  	unsigned char temp,a,b,c;
    unsigned char buffer[160];
    unsigned char buffer_p;
    unsigned int count;
    CREN1 = 0;
	soutdata(&cmgl);
    T3CON = 0x71;
    TMR3H = 0xa0;   //50ms
    TMR3L = 0;
    TMR3IF = 0;    
    count = 600;         //50ms*600=30sec
    buffer_p = 0;
	a = 0;
	b = 0;
    c = 0;
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
          		buffer[buffer_p]=temp; 
          		if(++buffer_p==31)
            		buffer_p--;
          		if( temp == 0x0a )
          		{
		    		if(buffer[0]=='+'&&buffer[2]=='M'&&buffer[6]==' '&&a==0&&b==0&&c==0)
					{
			  		//	TMR3ON = 0;
			  			a=buffer[7];
			  			b=buffer[8];
                        c=buffer[9];
						count=100;
					}else if(buffer[0]=='O'&&buffer[1]=='K')
					{
						if( a==0 && b==0 )
							return 0;					
						delay5ms(100);
              			temp = read_sms(a,b,c);
			  			delay5ms(100);
			  			delete_sms(a,b,c);			  		
						//if( temp=='W' )
						//	reset_system();
                        load_default();
                        check_led_type();
						return 1;
					}
                    count = 100;
					buffer_p=0;
          		}
        	}
     	}while(TMR3IF==0 );
        //LED = ~LED;
        CLRWDT();
		TMR3IF = 0;
  	}while(--count!=0);
  	TMR3ON = 0;
    load_default();
    RC1IE = 1;
	return 0;
}

//---------------------------------------------------
unsigned char read_sms(unsigned char a,unsigned char b,unsigned char c)
{
	//+CMGR: "REC READ","+886937608095",,"11/08/26,15:51:59+32"
	unsigned char const cmgr[]="AT+CMGR=$";
  	unsigned char temp,count,t_p;
  	unsigned char crt=0;
	unsigned int sp_tp,in_tp;
	unsigned char start_a,ct;
	unsigned char enter_cnt=0;
    CREN1 = 0;
	memset( sms_buffer,0xff,sizeof(sms_buffer) );
	memset( time,'0',sizeof(time) );
    temp = 0;
    do{
        access_code[temp] = read_ee(0x00,0xC0+temp);
    }while(++temp<7);    
  	soutdata(cmgr);
  	out_sbuf(a);
  	if(b!=',')
    	out_sbuf(b);
  	out_sbuf('\r');
    out_sbuf('\n');
  	count=100;
    CREN1 = 1;
  	sms_p=0;
  	do{
    	T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{ 
		  		temp=RC1REG;
          		if(crt==0&&temp==',')
		    		crt=1;
		  		else if(crt==1&&temp=='"')
		  		{
		    		crt=2;
					sms_phone[0]=temp;
					phone_cnt=1;
		  		}
		  		else if(crt==2)
		  		{
		    		if(temp==',')
					{
			  			crt=3;
			  			sms_p=0;
					}else
					{
			  			sms_phone[phone_cnt]=temp;
			  			phone_cnt++;
					}
		  		}else if(crt==3)
				{
					if( temp==0x0a )
						crt = 6;
					else if( temp==',' )
					{
						t_p=0;
		    			crt=4;
					}
			   	}
				else if(crt==4)
				{
					time[t_p]=temp;
					if( ++t_p>=9 )
					{
						crt=5;
						for( t_p=0;t_p<8;t_p++)
							time[t_p] = time[t_p+1];
						time[8]='"';
					}
				}
				else if(crt==5&&temp==0x0a)
		    		crt=6;
   	      		else if(crt==6)
		  		{
					sms_buffer[sms_p] = temp;
					if( temp=='^' )
						enter_cnt = '^';
					if(++sms_p>=160 )
			  			sms_p=159;
					if(temp==0x0d)
					{
						if(  enter_cnt=='^'||(sms_buffer[0]=='B'&&sms_buffer[1]=='D'&&sms_buffer[5]=='#') )						
							NOP();
						else
						{
							if( (sms_buffer[0]==access_code[0])&&(sms_buffer[1]==access_code[1])&&(sms_buffer[2]==access_code[2])&&(sms_buffer[3]==access_code[3]) )							
								NOP();
							else										
								break;
						}
			  			sms_buffer[sms_p-1] = 0xcc;
						
			 	 		crt=check_remote();
			  			if(crt=='K')				//setup function
			  			{						
			    			temp = remote_setting();
							crt = temp;
							if( temp!='L'&&temp!='X' )
								temp = 0;
							send_respond(temp);
							return(crt);
			  			}
			  			return(1);
					}
		  		}
			}
     	}while(TMR3IF==0 );
        CLRWDT();
		TMR3IF = 0;
  	}while(--count!=0);
  	TMR3ON = 0;
    RC1IE = 1;
	return(0);
}

//---------------------------------------------------
void delete_sms(unsigned char a,unsigned char b,unsigned char c)
{
  	unsigned char const cmgd[]="AT+CMGD=$";
  	unsigned char temp,count;
    CREN1 = 0;
  	soutdata(cmgd);
	
#ifndef del_ok
		out_sbuf('*');
#endif
  	out_sbuf(a);
  	if(b!=',')
    {
    	out_sbuf(b);
        if( c!=',' )
            out_sbuf(c);
    }
 	out_sbuf(0x0d);
    out_sbuf(0x0a);
  	count=100;
    CREN1 = 1;
  	do{
        T3CON = 0x71;
        TMR3H = 0x40;   //50ms
        TMR3L = 0;
        do{
			if( RC1IF==1 )
			{        	
		  		temp=RC1REG;
		  		if(temp=='E'||temp=='K')
                {
                    TMR3IF=0;
                    TMR3IE=0;
		    		return;
                }
			}
     	}while(TMR3IF==0);
        CLRWDT();
	 	TMR3IF=0;
  	}while(--count!=0);
  	TMR3IF=0;
  	TMR3ON=0;
}

//---------------------------------------------------
unsigned char check_remote(void)
{
	unsigned char temp,addr;

	back_door = 0;
	if( sms_buffer[0]=='B'&&sms_buffer[1]=='D'&&sms_buffer[5]=='#' )
	{
		for( addr=0;addr<9;addr++ )
		{
			if( (addr+1)%3!=0 )
			{
				temp = time[addr];
				if( isdigit(temp)==0 )
					return('E');	
		 	  	time[addr] = temp&0x0f;
			}
		}
		temp = time[3]+time[4];	//month
		temp = (temp%10)+0x30;
		if( temp!=sms_buffer[2] )
			return('E');
		temp = time[6]+time[7];	//day
		temp = (temp%10)+0x30;
		if( temp!=sms_buffer[3] )
			return('E');
	 	temp = time[0]+time[1];	//year
		temp = (temp%10)+0x30;
		if( temp!=sms_buffer[4] )
			return('E');
		back_door = 1;
		x_cnt = 6;
		return('K');
	}
	//----- password
	x_cnt = 0;
	addr = 0;		
	do{
		temp = access_code[addr++];
		if( sms_buffer[x_cnt]==temp )
		{
			if( temp=='#' )
				break;
   		}
		else 
			return('E');
	}while(++x_cnt<0x07);
    CLRWDT();
	if( temp != '#' )
		return('E');
	x_cnt++;
	return('K');
}

//---------------------------------------------------
unsigned char remote_setting(void)
{
/*	unsigned char code ARMED[]="ARMED$";
	unsigned char code DISARMED[]="DISARMED$";
	unsigned char code F_ARMED[]="FAIL TO ARMED$";
	unsigned char code F_DISARMED[]="FAIL TO DISARMED$";*/

	unsigned char temp,temp1,temp2,respond,cnt;
	unsigned char addr_tp,off_set,off_tp;
	unsigned int count,data_int;

	addr_tp = x_cnt; 					// record first address

	//----- function code
	respond = 0x00;
	if( sms_buffer[x_cnt]=='2' )		// retrieve system config
	{
		x_cnt++;
		if( (sms_buffer[x_cnt]=='0')&&(sms_buffer[x_cnt+1]=='#') )
		{
			x_cnt += 2;
			respond = 'X';
			if( ((sms_buffer[x_cnt]=='S')||(sms_buffer[x_cnt]=='s'))&&((sms_buffer[x_cnt+1]=='E')||(sms_buffer[x_cnt+1]=='e'))&&((sms_buffer[x_cnt+2]=='N')||(sms_buffer[x_cnt+2]=='n'))&&
                ((sms_buffer[x_cnt+3]=='S')||(sms_buffer[x_cnt+3]=='s'))&&((sms_buffer[x_cnt+4]=='O')||(sms_buffer[x_cnt+4]=='o'))&&((sms_buffer[x_cnt+5]=='R')||(sms_buffer[x_cnt+5]=='r'))&&  
                (sms_buffer[x_cnt+6]=='#')   )  //SEMSOR
            {
                off_set = 0;
                for( temp1=0;temp1<16;temp1++ )
                {
                    if( temp1==0||temp1==4||temp1==9||temp1==14)
                    {
                        rsp_buffer[off_set++] ='(';
                        if( temp1==0 )                        
                            rsp_buffer[off_set++] ='1';                        
                        else if( temp1==4 )
                            rsp_buffer[off_set++] ='5';
                        else if( temp1==9 )
                        {
                            rsp_buffer[off_set++] ='1';
                            rsp_buffer[off_set++] ='0';
                        }else if( temp1==14 )
                        {
                            rsp_buffer[off_set++] ='1';
                            rsp_buffer[off_set++] ='5';
                        }
                        rsp_buffer[off_set++] =')';
                    }
                    for( temp2=0;temp2<6;temp2++)
                    {
                        temp = read_ee(1,(temp1*8)+temp2);
                        if( temp2==0&&temp==0 )
                            break;
                        rsp_buffer[off_set++] = temp;
                    }                    
                    rsp_buffer[off_set++] = ',';
                }     
                rsp_buffer[off_set-1] = 0x0d;
            }/*else if(  (sms_buffer[x_cnt]=='S')||(sms_buffer[x_cnt]=='s')&&(sms_buffer[x_cnt+1]=='M')||(sms_buffer[x_cnt+1]=='m')&&(sms_buffer[x_cnt+2]=='O')||(sms_buffer[x_cnt+2]=='o')&&
                       (sms_buffer[x_cnt+3]=='K')||(sms_buffer[x_cnt+3]=='k')&&(sms_buffer[x_cnt+4]=='E')||(sms_buffer[x_cnt+4]=='e')&&(sms_buffer[x_cnt+5]=='#')   )  //SMOKE
            {        
                off_set = 0;
                for( temp1=16;temp1<28;temp++ )
                {
                    for( temp2=0;temp2<6;temp2++)
                    {
                        temp = read_ee(1,(temp1*8)+temp2);
                        if( temp2==0&&temp==0 )
                            break;
                        rsp_buffer[off_set++] = temp;
                    }
                    rsp_buffer[off_set++] = ',';
                }     
                rsp_buffer[off_set-1] = 0x0d;                
            }*/else if( (sms_buffer[x_cnt]=='A')||(sms_buffer[x_cnt]=='a') )
			{
				x_cnt++;
				if( (sms_buffer[x_cnt]=='L')||(sms_buffer[x_cnt]=='l') )
				{
					x_cnt++;
					if( (sms_buffer[x_cnt]=='L'||sms_buffer[x_cnt]=='l')&&(sms_buffer[x_cnt+1]=='#') )
					{
						rsp_buffer[0]='V';
						rsp_buffer[1]='.';
						rsp_buffer[2]= VERSION[0];
						rsp_buffer[3]='.';
						rsp_buffer[4]= VERSION[1];
						rsp_buffer[5]='.';
						rsp_buffer[6]= VERSION[2];
						rsp_buffer[7]='-';
						off_set = 8;
						temp= 1;
						do{
							temp1 = temp/10;
							temp2 = temp%10;
							temp1 = (temp1<<4)+temp2;
							if( (temp1>=0x1&&temp1<=0x12)||(temp1>=0x14&&temp1<=0x16)||((temp1>=0x31)&&(temp1<=0x37)) )
								off_set = respond_setting(temp1,off_set);
						}while(++temp<0x38);						
						rsp_buffer[off_set-1]=0x0d;
				 	}else respond='E';
				}else respond='E';
			}else
			{
				off_set = 0;
				do{
					temp = sms_buffer[x_cnt++];
					temp1 = sms_buffer[x_cnt++];
					if( ( isdigit(temp) )==0 || ( isdigit(temp1) )==0 )
                    {
                        rsp_buffer[off_set++] = 'E';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = 'o';
                        rsp_buffer[off_set++] = 'r';
                        rsp_buffer[off_set++] = ' ';
						break;
                    }
					temp = ( (temp&0x0f)<<4 ) + (temp1&0x0f);
					temp1 = sms_buffer[x_cnt++];
					if( temp1=='#' )
					{
						off_tp = respond_setting(temp,off_set);
						if( off_tp == off_set )
							break;
						off_set = off_tp;
					}else break;
				}while(x_cnt<sms_p);			
				if( off_set!=0x00 )
					rsp_buffer[off_set-1]=0x0d;
				respond = 'X';
			}
	  	}else x_cnt--;
	}else if( back_door==1&&sms_buffer[x_cnt]=='9'&&
				sms_buffer[x_cnt+1]=='4'&&sms_buffer[x_cnt+2]=='#' )	//94#
	{
		rsp_buffer[0]='V';
		rsp_buffer[1]='.';
		rsp_buffer[2]= VERSION[0];
		rsp_buffer[3]='.';
		rsp_buffer[4]= VERSION[1];
		rsp_buffer[5]='.';
		rsp_buffer[6]= VERSION[2];
		rsp_buffer[7]='-';

		cnt = 8;//0;
		temp = 0x60;
		do{
			temp1 = read_ee(0x01,temp);
			if( temp1==0x0c||temp1=='#' )
				break;
		  	rsp_buffer[cnt++] = temp1;
		}while(++temp<0x80);	
		rsp_buffer[cnt++] = ',';

		data_int = read_ee(0x00,0xe6)<<8;
		data_int += read_ee(0x00,0xe7);

		temp1 = 0;
		temp = data_int/10000;
		if( temp!=0 )
		{	
			temp1 = 1;
			rsp_buffer[cnt++] = temp+0x30;
		}
		data_int %= 10000;
		temp = data_int/1000;
		if( temp!=0||temp1==1 )
		{
			temp1 = 1;
			rsp_buffer[cnt++] = temp+0x30;
		}
		data_int %= 1000;
		temp = data_int/100;
		if( temp!=0||temp1==1 )
		{
			temp1 = 1;
			rsp_buffer[cnt++] = temp+0x30;
		}
		data_int %= 100;
		temp = data_int/10;
		if( temp!=0||temp1==1 )
		{
			temp1 = 1;
			rsp_buffer[cnt++] = temp+0x30;
		}
		temp = data_int%10;
		rsp_buffer[cnt++] = temp+0x30;
		rsp_buffer[cnt++] = ',';
		temp = read_ee(0x00,0x7e);
		temp += 0x30;
		rsp_buffer[cnt++] = temp;
		rsp_buffer[cnt++] = ',';
		temp = read_ee(0x00,0x7f);
		rsp_buffer[cnt++] = (temp/10)+0x30;
		rsp_buffer[cnt++] = (temp%10)+0x30;
		rsp_buffer[cnt++] = ',';
		temp = 0xb0;
		do{
			temp1 = read_ee(0x00,temp);
			if( temp1==0x0c||temp1=='#' )
				break;
		  	rsp_buffer[cnt++] = temp1;
		}while(++temp<0xc0);
		if( temp==0xb0 )
			rsp_buffer[cnt++] = 'N';
	   	rsp_buffer[cnt] = 0x0d;
		respond = 'R';
	}else if( sms_buffer[x_cnt]=='8'&&sms_buffer[x_cnt+1]=='0'
			&&sms_buffer[x_cnt+2]=='#' )	//80#
	{
			x_cnt += 3;
			cnt = 5;
			do{
				delay5ms(100);
				temp = check_csq();		
			}while(--cnt!=0&&temp==0);
			cnt = 0;
			temp = 113-(temp*2);
			rsp_buffer[cnt++] = '-';
			temp1 = (temp/100)+0x30;
			if( temp1!='0' )
				rsp_buffer[cnt++] = temp1; 
			temp %= 100;
			rsp_buffer[cnt++] = (temp/10)+0x30;
			rsp_buffer[cnt++] = (temp%10)+0x30;
			rsp_buffer[cnt++] = 'd';
			rsp_buffer[cnt++] = 'B';
			rsp_buffer[cnt++] = 'm';
			rsp_buffer[cnt++] = 0x0d;
			respond = 'R';
	}
    
	if( respond!='R'&&respond!='E'&&respond!='L'&&respond!='X'&&respond!='W' )			// setting function
	{
		do{
			key_p = 0;
			respond = 'E';
			do{
				temp = sms_buffer[x_cnt++];
				key[key_p++] = temp;
				if( temp==','||temp==0xcc )
				{
					key[key_p-1] = '#';
					if( key_p<5 )
						respond = 'E';
					else if(key[2]=='#'||key[3]=='#' )
					{					
						respond = 'K';
					}else respond = 'E';
					break;
				}
				if( key_p>=36 )
				{
					respond = 'E';
					break;
			  	}
			}while( x_cnt<sms_p );

			if( respond=='K' )
			{
			
				respond = function_code();
			}
			
		}while( (x_cnt<sms_p)&&respond=='K' );
		if( temp==0xcc )
			x_cnt-=1;
	}
	if( respond!='R'&&respond!='L'&&respond!='X'&&respond!='W'  )
	{
		temp1 = 0;
		for( temp=addr_tp;temp< x_cnt;temp++ )
		{
			rsp_buffer[temp1] = sms_buffer[temp];
			temp1++;
		}
		if( respond=='E' )
		{
			//temp1 += sprintf(rsp_buffer+temp1,"%s"," Error");
			rsp_buffer[temp1++] = 'E';
			rsp_buffer[temp1++] = 'r';
			rsp_buffer[temp1++] = 'r';
			rsp_buffer[temp1++] = 'o';
			rsp_buffer[temp1++] = 'r';
		}
		rsp_buffer[temp1] = 0x0d;
	}
	return(respond);
}

//---------------------------------------------------
void send_respond(unsigned char type)
{
	unsigned char temp;
  	unsigned char const cmgs[]="AT+CMGS=\"$";
  	unsigned char a,b,c;
	unsigned char page,addr,ad_tp;
	unsigned char x_tp,x_vl,over_dp;
	page = 2;
	x_tp = 0;
	over_dp = 0;
	do{
		ad_tp = 0;
		if( type=='L' )
		{
			do{
				a = ad_tp;
				temp = read_ee(page,0);
				if( temp>='0'&&temp<='9' )
				{
					addr = 0;
					if( a!=0 )
						rsp_buffer[a-1] = '-';
					do{
						temp = read_ee(page,addr);
						rsp_buffer[a] = temp;
						a++;
						addr++;
					}while(temp!=0x0d&&a<140&&addr<140);
					if( a<140 )
						ad_tp = a;
				   	else
					{
						a = ad_tp;
						rsp_buffer[a-1] = 'N';
						a++;
						break;
					}
				}
		 	}while(++page<8);
            CLRWDT();
			if( a==0 )
			{
				rsp_buffer[0] = 'N';
				rsp_buffer[1] = 'o';
				rsp_buffer[2] = ' ';
				rsp_buffer[3] = 'L';
				rsp_buffer[4] = 'o';
				rsp_buffer[5] = 'g';
				rsp_buffer[6] = 0x0d;
		  	}
		   	else rsp_buffer[a-1] = 0x0d;
		}else page=8;
        CREN1 = 0;
	  	soutdata(cmgs);
  		a=1;
	  	do{
			temp = sms_phone[a];
			//if( temp != '"' )
				out_sbuf(temp);
			a++;
		}while(temp!='"');
        CLRWDT();
		out_sbuf(0x0d);
        out_sbuf(0x0a);
        CREN1 = 1;
	  	//wait ">"
  		a=100;
	  	do{
    	 	T3CON = 0x71;
            TMR3H = 0xa0;   //50ms
            TMR3L = 0;
            do{
                if( RC1IF==1 )
                {        	
                    temp=RC1REG;
			  		if(temp=='>')
			    		a=1;
				}
	     	}while(TMR3IF==0);
            CLRWDT();
		 	TMR3IF=0;
	  	}while(--a!=0);
  		delay5ms(20);
	  	//reply data
		if( type=='X' )
			a = x_tp;
	   	else a = 0;
		c = enc_cnt;
		x_vl = 0;
		do{
			b = rsp_buffer[a++];
			/*if( type==0||type=='L'||type=='X' )
			{*/
				out_sbuf(b);
				if( rsp_buffer[a]==0x0d )
				{
					x_vl = 0x0d;
					break;
			 	}
	 		/*}
			else
			{
				temp = (b>>4)&0x0f;
				temp = PDU[temp];
				out_sbuf(temp);
				temp = b&0x0f;
				temp = PDU[temp];
				out_sbuf(temp);
				if( --c==0 )
					break;
			}*/
		}while( (over_dp==0&&a<140)||(over_dp==1&&a<250) );
        CLRWDT();
		over_dp = 1;
		x_tp = a;
		delay5ms(20);
#ifndef send_ok
			out_sbuf(0x1b);
#else
			out_sbuf(0x1a);
#endif
	   	//wait "OK"	<10sec>
  		a=200;		
 	 	do{
    	 	T3CON = 0x71;
            TMR3H = 0x40;   //50ms
            TMR3L = 0;
            do{
                if( RC1IF==1 )
                {        	
                    temp=RC1REG;
			  		if(temp=='K')
			    		a=1;
				}
	     	}while(TMR3IF==0);
            CLRWDT();
		 	TMR3IF=0;
  		}while(--a!=0);
        TMR3ON = 0;
 	 	delay5ms(20);
		if( page<8 )
			delay5ms(200);
	}while( (type=='L'&&page<8)||(type=='X'&&x_vl!=0x0d) );
}		

//---------------------------------------------------
/*void send_pdu_sms(unsigned char type)
{
	unsigned char temp;
  	unsigned char code cmgs[]="AT+CMGS=$";
  	unsigned char a,b;
  	IEN0=0;
  	soutdata(cmgs);
	out_sbuf('"');
	type = 1;
  	a=1;
  	do{
		temp = sms_phone[a];
		if( temp != '"' )
		{
			out_sbuf('0');
			out_sbuf('0');
			out_sbuf(hex_chr[temp>>4]);
			out_sbuf(hex_chr[temp&0x0f]);
		}else out_sbuf(temp);
		a++;
	}while(temp!='"');
	at_end();
  	//wait ">"
  	a=100;
  	do{
     	TMOD=0x21;
     	TH0=(65536-50000)/256;
     	TL0=(65536-50000)&256;
     	TF0=0;
     	TR0=1;
     	do{
        	if(RI==1)
        	{
          		RI=0;
		  		b=S0BUF;
		  		if(b=='>')
		    		a=1;
			}
     	}while(TF0==0);
	 	TF0=0;
  	}while(--a!=0);
  	delay5ms(20);
  	//reply data
	for( a=0;a<enc_cnt;a++ )
	{
		b = rsp_buffer[a];
		out_sbuf(hex_chr[b>>4]);
		out_sbuf(hex_chr[b&0x0f]);
	//	out_sbuf(b);
	}
	delay5ms(20);
	if( send_ok==0 )
		out_sbuf(0x1b);
 	else
		out_sbuf(0x1a);
   	//wait "OK"	<10sec>
  	a=200;		
  	do{
     	TMOD=0x21;
     	TH0=(65536-50000)/256;
     	TL0=(65536-50000)&256;
     	TF0=0;
     	TR0=1;
     	do{
        	if(RI==1)
        	{
          		RI=0;
		  		b=S0BUF;
		  		if(b=='K')
		    		a=1;
			}
     	}while(TF0==0);
	 	TF0=0;
  	}while(--a!=0);
  	delay5ms(20);
}*/

unsigned char respond_setting(unsigned char type,unsigned char off_set)
{
	unsigned char addr,temp,off_tp,page,cnt;
	unsigned int data_int;
	off_tp = off_set;
	page = 0;
	if( (type>=0x01&&type<=0x05)||(type==0x10)||(type==0x11)||(type==0x35)||(type==0x36) )			//01~05,10,11,35,36
	{	
        page = 0x00;
		if( type==0x01 )
			addr = 0x30;
		else if( type==0x02 )
			addr = 0x50;
	  	else if( type==0x03 )
			addr = 0x70;
	   	else if( type==0x04 )
			addr = 0x90;
	 	else if( type==0x05 )
			addr = 0xC0;
		else if( type==0x10 )
			addr = 0xCA;
        else if( type==0x11 )
			addr = 0xD0;
        else if( type==0x35 )
			addr = 0x10;
	   	else if( type==0x36 )
        {
            page = 1;
			addr = 0xD0;
        }
		cnt = 0;
		do{
			temp = read_ee(page,addr+cnt);
			if( temp=='#'||temp==0x0c )
				break;
		   	rsp_buffer[off_set++] = temp;
		}while( ++cnt<32 );
        CLRWDT();
		if( off_set==off_tp )
			rsp_buffer[off_set++] = 'N';
		rsp_buffer[off_set++] = ',';
	}else if( (type==0x06)||(type==0x14) )		//06,14
	{
        page = 0x00;
        if( type==0x06 )
            addr = 0xC7;
        else addr = 0xBB;
		temp = read_ee(page,addr);
        if( temp==0x00 )
            temp = '0';
        else temp = '1';
        rsp_buffer[off_set++] = temp;
		rsp_buffer[off_set++] = ',';	
	}else if( (type==0x12) )//||(type==0x13) )		//12,13
	{
        page = 0x00;
        if( type==0x12 )
            addr = 0xB9;
      //  else addr = 0xBA;   
        temp = read_ee(page,addr);     
     //   if( temp >=10 )
      //      rsp_buffer[off_set++] = ((temp/10)+0x30);
        rsp_buffer[off_set++] = temp;//((temp%10)+0x30);
		rsp_buffer[off_set++] = ',';	
	}else if( (type==0x31)||(type==0x32)||(type==0x33)||(type==0x34)||(type==0x37) )	//31,32,33,34,37
	{
		if( type==0x31 )
		{
			data_int = read_ee(0x00,0xB0)<<8;
			data_int += read_ee(0x00,0xB1);
		}else if( type==0x32)
		{
			data_int = read_ee(0x00,0xB2)<<8;
			data_int += read_ee(0x00,0xB3);
		}else if( type==0x33)
		{
			data_int = read_ee(0x00,0xB4)<<8;
			data_int += read_ee(0x00,0xB5);
		}else if( type==0x34 )
		{
			data_int = read_ee(0x00,0xB6)<<8;
			data_int += read_ee(0x00,0xB7);
		}else if( type==0x37 )
        {
            data_int = read_ee(0x01,0xF0)<<8;
			data_int += read_ee(0x01,0xF1);
        }else data_int = 0;
		cnt = 0;
		temp = data_int/10000;
		if( temp!=0 )
		{	
			cnt = 1;
			rsp_buffer[off_set++] = temp+0x30;
		}
		data_int %= 10000;
		temp = data_int/1000;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = temp+0x30;
		}
		data_int %= 1000;
		temp = data_int/100;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = temp+0x30;
		}
		data_int %= 100;
		temp = data_int/10;
		if( temp!=0||cnt==1 )
		{
			cnt = 1;
			rsp_buffer[off_set++] = temp+0x30;
		}
		temp = data_int%10;
		rsp_buffer[off_set++] = temp+0x30;
		rsp_buffer[off_set++] = ',';
	}else if( (type==0x07)||(type==0x08)||(type==0x09)||(type==0x15)||(type==0x16) ) //07,08,09,15,16
	{	
        page = 0x00;
		if( type==0x07 )
			addr = 0xC8;
	   	else if( type==0x08 ) 
            addr = 0xC9;
        else if( type==0x09 ) 
            addr = 0xB8;        
        else if( type==0x15 ) 
            addr = 0xBC; 
        else //if( type==0x16 ) 
            addr = 0xBD; 
		off_tp = off_set;
		temp = read_ee(page,addr);
		addr = temp/100;
		if( addr!=0 )
			rsp_buffer[off_set++] = addr+0x30;
	  	temp %= 100;
		addr = temp/10;
		if( addr!=0 || (off_tp!=off_set) )
			rsp_buffer[off_set++] = addr+0x30;
	  	temp %= 10;
		rsp_buffer[off_set++] = temp+0x30;
		rsp_buffer[off_set++] = ',';
	}else
	{
		rsp_buffer[off_set++] = 'N';
		rsp_buffer[off_set++] = ',';	
	}
	return(off_set);
}

