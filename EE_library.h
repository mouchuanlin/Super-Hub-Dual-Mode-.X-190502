/* 
 * File:   EE_library.h
 * Author: YEN
 *
 * Created on 2017年4月12日, 下午 2:14
 */

#ifndef EE_LIBRARY_H
#define	EE_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif


unsigned char read_ee(unsigned char ,unsigned char );
void write_ee(unsigned char ,unsigned char ,unsigned char );

#ifdef	__cplusplus
}
#endif

#endif	/* EE_LIBRARY_H */

