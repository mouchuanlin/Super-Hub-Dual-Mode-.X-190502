/* 
 * File:   SMS_library.h
 * Author: YEN
 *
 * Created on 2017年4月28日, 下午 3:35
 */

#ifndef SMS_LIBRARY_H
#define	SMS_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif
void set_sms_init(void);
unsigned char check_sms(void);
unsigned char read_sms(unsigned char,unsigned char,unsigned char);
void delete_sms(unsigned char,unsigned char,unsigned char);
unsigned char check_remote(void);
unsigned char remote_setting(void);
void send_respond(unsigned char);
void select_sms_type(bit);
unsigned char respond_setting(unsigned char,unsigned char);



#ifdef	__cplusplus
}
#endif

#endif	/* SMS_LIBRARY_H */

