/* 
 * File:   Module_LB_Telit.h
 * Author: YEN
 *
 * Created on 2018年1月15日, 下午 4:22
 */

#ifndef MODULE_LB_TELIT_H
#define	MODULE_LB_TELIT_H

#ifdef	__cplusplus
extern "C" {
#endif

void TL_module_first_run(void);
unsigned char TL_internet_init(void);
unsigned char TL_connection_open(unsigned char type);
void TL_connection_close(void);
void TL_internet_close(void);
unsigned char TL_send_data_to_server(void);
unsigned char TL_receive_data_from_server(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LB_TELIT_H */

