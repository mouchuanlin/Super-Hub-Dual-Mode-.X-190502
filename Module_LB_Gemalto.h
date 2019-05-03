/* 
 * File:   Module_LB_Gemalto.h
 * Author: YEN
 *
 * Created on 2018年1月15日, 下午 4:22
 */

#ifndef MODULE_LB_GEMALTO_H
#define	MODULE_LB_GEMALTO_H

#ifdef	__cplusplus
extern "C" {
#endif

void GM_module_first_run(void);
unsigned char GM_internet_init_EMS31(void);
unsigned char GM_connection_open(unsigned char type);
void GM_connection_close(void);
void GM_internet_close(void);
unsigned char GM_send_data_to_server(void);
unsigned char GM_receive_data_from_server(void);



#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LB_GEMALTO_H */

