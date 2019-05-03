/* 
 * File:   emc_library.h
 * Author: YEN
 *
 * Created on 2017年5月2日, 下午 2:10
 */

#ifndef EMC_LIBRARY_H
#define	EMC_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif


bit alarm_out(unsigned char type,unsigned char zone_ext)@0xA000;
void add_even(unsigned char even,unsigned char zone);
void check_even(void);
void load_emc_number(void);
unsigned char check_emc_stack(void);
unsigned char chk_data_type(unsigned char *buffer,unsigned char buffer_p);

#ifdef	__cplusplus
}
#endif

#endif	/* EMC_LIBRARY_H */

