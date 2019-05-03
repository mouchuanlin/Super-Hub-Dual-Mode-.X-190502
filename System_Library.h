/* 
 * File:   System_Library.h
 * Author: YEN
 *
 * Created on 2017年4月12日, 下午 2:38
 */

#ifndef SYSTEM_LIBRARY_H
#define	SYSTEM_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif


void delay5ms(unsigned int);
void first_run(void);
void load_default(void);
void check_led_type(void);
void loed_ID_to_buffer(void);
unsigned char check_ID(unsigned char *ptr);
unsigned char add_ID(unsigned char *ptr);
unsigned char del_ID(unsigned char id);
unsigned char check_supervisory(void);
unsigned char send_trigger_to_RF(unsigned char type);


#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_LIBRARY_H */

