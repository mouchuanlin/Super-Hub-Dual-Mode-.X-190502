#include "initial.h"
#include <stdlib.h>
#include "EE_library.h"
#include "System_Library.h"
#include "Module_Library.h"
#include "Module_LB_Gemalto.h"
#include "Module_LB_Telit.h"
#include <pic18f26k22.h>
#include <xc.h>

void update_mmcnt(void)
{
	unsigned char mm_cnt;
	mm_cnt = read_ee(0,0x0f);

	if( ++mm_cnt>=END_MM_COUNT )
		mm_cnt = START_MM_COUNT;
	write_ee(0x00,0x0f,mm_cnt);
}

unsigned char save_stack_header(void)
{
	unsigned char tp_cnt,temp,cnt1,cnt2;

	tp_cnt = 0;
	do{
		if( stack_buffer[tp_cnt][0]==0x00 )
		{
			stack_buffer[tp_cnt][0] = 'P';
			stack_buffer[tp_cnt][1] = 1;				//dial type
			temp = (read_ee(0x00,0x7b)<<4);				//line fault
			/* new add 12/09 */
			if( read_ee(0x01,0x20)==0x0c )				//retry count if line2 empty,line1 retry <5>
				temp += RETRY_NUMBER1;	
			else temp += RETRY_NUMBER2;	
			/*---------------*/
			stack_buffer[tp_cnt][2] = temp;

			temp = read_ee(0x00,0x59);
			stack_buffer[tp_cnt][3] = temp;			 	//ack time
			stack_buffer[tp_cnt][4] = temp;				//ack count
            stack_buffer[tp_cnt][5] = 3;                // reboot times
            stack_buffer[tp_cnt][6] = 3;
		/*	cnt1 = 0;
			cnt2 = 5;
			do{
				temp = key[cnt1++];
				if( temp!=0x0c )
					temp = adjust_ascii(temp);
				stack_buffer[tp_cnt][cnt2++] = temp;
			}while( temp!=0x0c && cnt1<16 );*/
							
			update_mmcnt();	
			return(tp_cnt);
		}
	}while(++tp_cnt<BUFFER_STACK);
    CLRWDT();
	return(tp_cnt);
}

unsigned char stack_data_header(unsigned char tp_cnt,unsigned char mm_cnt,unsigned char cnt)
{
	unsigned char cnt2,temp;
	unsigned char const hex[17]="0123456789ABCDEF";
	//temp = (mm_cnt/10)+0x30;			//MM
	temp = hex[(mm_cnt>>4)];
	stack_buffer[tp_cnt][cnt++] = temp;
	//temp = (mm_cnt%10)+0x30;
	temp = hex[(mm_cnt&0x0f)];
	stack_buffer[tp_cnt][cnt++] = temp;
	cnt2 = 0xD0;						//L
	do{
		temp = read_ee(0x00,cnt2);
		if( temp!='#'&& temp!=0x0c)
			stack_buffer[tp_cnt][cnt++] = temp;
		else temp='#';
	}while( ++cnt2<0xD5&&temp!='#' );
	stack_buffer[tp_cnt][cnt++] = ',';					//,
	cnt2 = 0xCA;								//unit accnt
	do{	
		temp = read_ee(0x00,cnt2);		
		if( temp!='#' )
			stack_buffer[tp_cnt][cnt++] = temp;
	}while( ++cnt2<0xD0&&temp!='#' );
    CLRWDT();
	return(cnt);
}

//------------------------------//
void move_stack_buffer(void)
{
	unsigned char cnt,temp,cnt2;
    for( cnt = 0;cnt<(BUFFER_STACK-1);cnt++ )
    {
        cnt2 = 0;
        do{
            temp = stack_buffer[cnt+1][cnt2];
            stack_buffer[cnt+1][cnt2] = 0;
            stack_buffer[cnt][cnt2] = temp;
        }while(++cnt2<LOG_MAX_T);
        CLRWDT();
    }                
    
/*	for( cnt=0;cnt<LOG_MAX_T;cnt++ )
	{
		stack_buffer[0][cnt] = 0;
  	}

    
  	if( stack_buffer[1][0] !=0 )
	{
		for( cnt=0;cnt<LOG_MAX_T;cnt++ )
		{
			temp = stack_buffer[1][cnt];
			stack_buffer[0][cnt] = temp;
	  	}
	}else 
	{
		//connection_close();
		//internet_close();
		return;
	}
	for( cnt=0;cnt<LOG_MAX_T;cnt++ )
	{
		stack_buffer[1][cnt] = 0;
  	}*/
}

//------------------------------//
void load_emc_number(void)
{
	unsigned char temp,cnt1,cnt2,temp1,random,swap,type,page;
	unsigned char check_sum;
	//-----mov stack_buffer_data[20~159] to rsp_buffer
	cnt1 = 20;
	cnt2 = 0;
	/*------------random number---------------*/
	if( encryption==1 )
	{
		random = (rand()>>8)^rand()^random_rx;
		rsp_buffer[cnt2++] = random;
		check_sum = random;
		random = ((random>>4)+(random&0x0f))%16;
	}
	/*------------encryption ---------------*/
	do{
		temp = stack_buffer[0][cnt1++];
		if( encryption==1 )
		{
			if( temp==0x0d )
				break;
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
			rsp_buffer[cnt2++] = temp;
			if( ++random>=16 )
				random = 0;
			/*-------------encryption --------------*/
			check_sum ^= temp;
	  	}else
		{
			rsp_buffer[cnt2++] = temp;
			if( temp==0x0d )
				break;
		}
	}while(cnt2< (LOG_MAX_T-20));
    CLRWDT();
	if( encryption==1 )
	{		
		rsp_buffer[cnt2++] = check_sum;
	}
	enc_cnt = cnt2;
}

bit alarm_out(unsigned char type,unsigned char zone_ext)@0xA000
{    
    unsigned char const SMOKE[]="18,111100$";//002$";
    unsigned char const TEST_T[]="18,E60200$";//035$";
	unsigned char const LBT[]="18,130200$";//002$";
    unsigned char const TP_O[]="18,138300$";//002$";
    unsigned char const TP_C[]="18,338300$";//002$";
    unsigned char const TEST_P[]="18,E60100$";//002";
    unsigned char const EOL_S[]="18,138600$";//002$";
#ifdef DEBUG
    unsigned char const LBT_S[]="18,130211$";//002$";
#else
    unsigned char const LBT_S[]="18,138400$";//002$";
#endif
    unsigned char const ELOOD_S[]="18,115400$";//002$";
    unsigned char const GLASS_S[]="18,114000$";//002$";   
    unsigned char const SUPER_S[]="18,E38100$";//002$";
    unsigned char const CARBON_S[]="18,116200$";//002$";
    unsigned char const MOTION_S[]="18,113200$";//002$";
    unsigned char const DOOR_S[]="18,1134700$";//002$";
    unsigned char const PANIC_S[]="18,110100$";//002$";
    unsigned char const HVAC_S[]="18,116100$";//002$";
    unsigned char const APPLIANCE_S[]="18,115200$";//002$";
    unsigned char const RESERVE1_S[]="18,115500$";//002$";
    unsigned char const RESERVE2_S[]="18,118000$";//002$";
    unsigned char const RESERVE3_S[]="18,118100$";//002$";
    unsigned char const RESERVE4_S[]="18,118200$";//002$";
    unsigned char const RESERVE5_S[]="18,118300$";//002$";
    unsigned char const RESERVE6_S[]="18,118400$";//002$";
    unsigned char const RESERVE7_S[]="18,118500$";//002$";
    unsigned char const GO_SMS_S[]="18,E30600$";//002$";
	unsigned char tp_cnt,mm_cnt,cnt,temp;
    unsigned char *sp;
    
#ifdef DEBUG    
    unsigned int zone;
    zone =  ADC_data;
#endif
    
    if( type== SMOKE_ALARM_T )    
        sp = &SMOKE[0];   
    else if( type== TEST_CYCLE_S )    
        sp = &TEST_T[0];    
    else if( type== LOW_BATTERY_T )    
        sp = &LBT[0];    
    else if( type==TAMPER_OPEN_T )    
        sp = &TP_O[0];    
    else if( type==TAMPER_CLOSE_T )    
        sp = &TP_C[0];    
    else if( type==TEST_PIN_T )
        sp = &TEST_P[0];    
    else if( type==EOL_T )    
        sp = &EOL_S[0];    
    else if( type==LOW_BATTERY_S )    
        sp = &LBT_S[0];    
    else if( type==FLOOD_T )    
        sp = &ELOOD_S[0];    
    else if( type==GLASS_T )    
        sp = &GLASS_S[0];  
    else if( type==HVAC_T )    
        sp = &HVAC_S[0];  
    else if( type==SUPERVISORY_T )    
        sp = &SUPER_S[0];  
    else if( type==CARBON_T )    
        sp = &CARBON_S[0];  
    else if( type==MOTION_T )    
        sp = &MOTION_S[0];      
    else if( type==DOOR_T )    
        sp = &DOOR_S[0];      
    else if( type==PANIC_T )    
        sp = &PANIC_S[0];      
    else if( type==APPLIANCE_T )    
        sp = &APPLIANCE_S[0];      
    else if( type==RESERVE1_T )    
        sp = &RESERVE1_S[0];      
    else if( type==RESERVE2_T )    
        sp = &RESERVE2_S[0];      
    else if( type==RESERVE3_T )    
        sp = &RESERVE3_S[0];      
    else if( type==RESERVE4_T )    
        sp = &RESERVE4_S[0];      
    else if( type==RESERVE5_T )    
        sp = &RESERVE5_S[0];      
    else if( type==RESERVE6_T )    
        sp = &RESERVE6_S[0];      
    else if( type==RESERVE7_T )    
        sp = &RESERVE7_S[0];  
    else if( type==GO_SMS_T )    
        sp = &GO_SMS_S[0];  
    else return(0);
 
    for( cnt=0;cnt<BUFFER_STACK;cnt++ )
    {
        if( stack_buffer[cnt][0]=='P')
            break;
    }
    if( cnt!=BUFFER_STACK )
    {      
        for( tp_cnt=20;tp_cnt<LOG_MAX_T;tp_cnt++)
        {
            if( stack_buffer[cnt][tp_cnt]==0x0d )
            {
                if( tp_cnt<160 )
                {
                    mm_cnt = 2;
                    do{
                        temp = sp[mm_cnt++];
                        stack_buffer[cnt][tp_cnt++] = temp;
                    }while(temp!='$');
                    #ifndef DEBUG
                    stack_buffer[cnt][tp_cnt-1] = read_ee(0x00,0xB9);        
                    stack_buffer[cnt][tp_cnt++] = (zone_ext/10)+0x30;
                    stack_buffer[cnt][tp_cnt++] = (zone_ext%10)+0x30;
                    #else
                    stack_buffer[cnt][tp_cnt-1] = (zone/100)+0x30;
                    zone %= 100;        
                    stack_buffer[cnt][tp_cnt++] = (zone/10)+0x30;
                    stack_buffer[cnt][tp_cnt++] = (zone%10)+0x30;
                    #endif
                    stack_buffer[cnt][tp_cnt++] = 0x0d;
                    return(1);
                }
            }
        }       
    }
    mm_cnt = read_ee(0,0x0f);
    tp_cnt = save_stack_header();
    if( type==TEST_PIN_T||type==TEST_CYCLE_S   )
    {
        stack_buffer[tp_cnt][0] = 'T';
        stack_buffer[tp_cnt][5] = 1;
        stack_buffer[tp_cnt][6] = 1;
    }
    if( tp_cnt < BUFFER_STACK )
    {
        stack_buffer[tp_cnt][1] = 1; //usb line1~4
        //MML,ACCT18,E60200002
        cnt = 20;
        cnt = stack_data_header(tp_cnt,mm_cnt,cnt);
        mm_cnt = 0;
        do{
            temp = sp[mm_cnt++];
            stack_buffer[tp_cnt][cnt++] = temp;
        }while(temp!='$');
#ifndef DEBUG
        stack_buffer[tp_cnt][cnt-1] = read_ee(0x00,0xB9);        
        stack_buffer[tp_cnt][cnt++] = (zone_ext/10)+0x30;
        stack_buffer[tp_cnt][cnt++] = (zone_ext%10)+0x30;
#else
        stack_buffer[tp_cnt][cnt-1] = (zone/100)+0x30;
        zone %= 100;        
        stack_buffer[tp_cnt][cnt++] = (zone/10)+0x30;
        stack_buffer[tp_cnt][cnt++] = (zone%10)+0x30;
#endif
        stack_buffer[tp_cnt][cnt++] = 0x0d; 
    }else return(0);            
	return(1);
}

unsigned char chk_data_type(unsigned char *buffer,unsigned char buffer_p)
{
	unsigned char cnt,temp1,temp2,random,swap;
	unsigned char i_cnt;
    //unsigned char tp_buffer[250];
    //unsigned char tp_cnt;
    
    buffer_p -= 2;    

	for( i_cnt=0;i_cnt<buffer_p;i_cnt++ )
	{
		temp1 = buffer[i_cnt];
		if( temp1==','||temp1==' ' )
		{
			if( buffer[0]>='0'&&buffer[0]<='9'&&buffer[1]>='0'&&buffer[1]<='9' )
			{							
				break;
			}
	 	}
	}	
	//decrypt
	if( i_cnt==buffer_p )		
	{		        
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
		buffer[cnt-1] = 0xcc;
		buffer_p = cnt;
	  	/*------------------*/
	}else 
    {
        if( buffer[buffer_p-1]==0x0d )
            buffer[buffer_p-1] = 0xcc;
        else buffer[buffer_p++] = 0xcc;                    
    }

                //------------------------
#ifdef DEBUG
    unsigned char const ascii[]="0123456789ABCDEF";
    delay5ms(200);
    delay5ms(200);
        delay5ms(200);
        soutdata("AT+Q=$");
        cnt = 0;
        do{
            temp1 = buffer[cnt];            
            out_sbuf(ascii[temp1>>4]);
            out_sbuf(ascii[temp1&0x0f]);
        }while(++cnt<buffer_p);
        out_sbuf(0x0d);
        out_sbuf(0x0a);
        delay5ms(200);
#endif            
	if( buffer[2]==' '&&buffer[3]=='N'&&buffer[4]=='A'&&buffer[5]=='C')//&&buffer[6]=='K' )
	{
		if( (buffer[0]==stack_buffer[0][20])&&(buffer[1]==stack_buffer[0][21]) )
		{
			stack_buffer[0][4] = 1;
			temp1 = stack_buffer[0][2]&0xf0;
			stack_buffer[0][2] = temp1;
			return('N');
	   	}
	}else
	{	
        #ifdef DEBUG
        soutdata("AT+C=$");
        #endif
		cnt = 0;
		do{
			temp1 = stack_buffer[0][cnt+20];
			temp2 = buffer[cnt];	
            #ifdef DEBUG
            out_sbuf(ascii[temp1>>4]);
            out_sbuf(ascii[temp1&0x0f]);			
            out_sbuf(ascii[temp2>>4]);
            out_sbuf(ascii[temp2&0x0f]);
            out_sbuf('-');
            #endif
			if( temp1!=temp2 )
			{
				if( temp1==0x0d&&temp2==0xcc )
				{			
					move_stack_buffer();
                    #ifdef DEBUG
                    out_sbuf('K');
                    out_sbuf(0x0d);
                    out_sbuf(0x0a);
                    delay5ms(200);
                    #endif
					return('K');
				}
                #ifdef DEBUG
                out_sbuf('E');
                out_sbuf(0x0d);
                out_sbuf(0x0a);
                delay5ms(200);
                #endif
				return('E');
			}
		}while(++cnt<=buffer_p);
	}
	return('E');
}

//------------------------------//
unsigned char check_emc_stack(void)
{
    unsigned char temp,cnt,rsp,loop,count;
#ifdef DEBUG
    unsigned char const ascii[]="0123456789ABCDEF";
#endif
    encryption = read_ee(0x00,0xE0);
    
send_start:
    //lock_buffer = 0;
    temp = stack_buffer[0][0];
    if( temp=='P'||temp=='S'||temp=='T' )     //connect to server
    {
      //  encryption = 0;
        GIE = 0;
        if( temp=='P' )
            stack_buffer[0][0] = 'S';
        GIE = 1;
        load_emc_number();				
       // stack_buffer[0][0] = 'S';
        //------------------------
#ifdef DEBUG
        delay5ms(200);
        soutdata("AT+Q=$");
        out_sbuf(ascii[enc_cnt>>4]);
        out_sbuf(ascii[enc_cnt&0x0f]);
        out_sbuf('-');
        cnt = 0;
        do{
            temp = rsp_buffer[cnt];            
            out_sbuf(ascii[temp>>4]);
            out_sbuf(ascii[temp&0x0f]);
        }while(++cnt<enc_cnt);
        out_sbuf(0x0d);
        out_sbuf(0x0a);
        delay5ms(200);
#endif
        //------------------------------------
        //TMR0IE = 0;
        delay5ms(200);
        if( GEMALTO )
        {
            if( Module_type==EHS5 )
                GM_internet_init();
            else GM_internet_init_EMS31();
        }
        else TL_internet_init();        
        cnt = 1;
        do{
            if(  stack_buffer[0][0]=='T' )
                loop = 1;
            else
                loop = read_ee(0x00,0xBC);
            do{
                CREN1 = 0;
                if( GEMALTO )
                    rsp = GM_connection_open(cnt);              
                else rsp = TL_connection_open(cnt);
                if( rsp=='K' )      //send data to server
                {
                    if( GEMALTO )
                        rsp = GM_send_data_to_server();      
                    else rsp = TL_send_data_to_server();     
                    if( rsp=='K' )     
                    {
                        count = read_ee(0x00,0xC9);
                        do{                            
                            delay5ms(200);
                            delay5ms(200);
                            delay5ms(200);
                            delay5ms(200);
                            delay5ms(200);
                            if( GEMALTO )
                                rsp = GM_receive_data_from_server();                   
                            else rsp = TL_receive_data_from_server();         
                            delay5ms(200);
                            delay5ms(200);
                            //------------------------
                            #ifdef DEBUG
                            soutdata("AT+U=$");
                            out_sbuf(ascii[debug_p>>4]);
                            out_sbuf(ascii[debug_p&0x0f]);
                            out_sbuf('-');
                            cnt = 0;
                            do{
                                temp = debug_buffer[cnt];
                          /*      if( temp!=0x0d&&temp!=0x0a )
                                {
                                    out_sbuf(temp);
                                }else
                                {                                    
                                    if( temp==0x0d )
                                        out_sbuf('r');
                                    else out_sbuf('n');
                                }*/                                   
                                    out_sbuf(ascii[temp>>4]);
                                    out_sbuf(ascii[temp&0x0f]);
                            }while(++cnt<debug_p);
                            out_sbuf(0x0d);
                            out_sbuf(0x0a);
                            #endif
                        }while(--count!=0&&rsp=='E');
                    }
                }else LED_flash_type = LED_NET_ERR;
                delay5ms(100);
                if( GEMALTO )
                    GM_connection_close();
                else TL_connection_close();
                delay5ms(200);
                if( stack_buffer[0][0]=='T'&&cnt==2 )
                    break;
            }while(--loop!=0&&rsp!='K'); 
        }while(++cnt<5&&rsp!='K');  
        if( GEMALTO )
            GM_internet_close();
        else TL_internet_close();
        if( rsp=='K' )		
        {
            //move_stack_buffer();
            delay5ms(200);
            #ifdef DEBUG
            soutdata("AT+OK=1\r\n$");
            delay5ms(200);
            #endif
            retry_count = 0;
            goto send_start;            
        }else retry_count = read_ee(0x00,0xBD)*600;
        stack_buffer[0][5]--;
        if( stack_buffer[0][5]==0 )
        {
            stack_buffer[0][5] = stack_buffer[0][6];
            return('E');
        }
        else return('U');
    }
}

void add_even(unsigned char even,unsigned char zone)
{
    even_log[even_count_f][0] = even;
    even_log[even_count_f][1] = zone;
    if( ++even_count_f>=EVEN_MAX )
        even_count_f = 49;
}

void check_even(void)
{
    do{
        alarm_out(even_log[even_count_l][0],even_log[even_count_l][1]);
        even_count_l++;
        if( even_count_l==even_count_f )
        {
            even_count_f = 0;
            even_count_l = 0;
        }
    }while(even_count_l!=even_count_f);
}