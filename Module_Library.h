/* 
 * File:   Module_Library.h
 * Author: YEN
 *
 * Created on 2017?3?22?, ?? 9:56
 */

#ifndef MODULE_LIBRARY_H
#define	MODULE_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

    void out_sbuf(unsigned char);
    void soutdata(unsigned char*);
    void out_sbuf2(unsigned char);
    unsigned char wait_ok_respond(unsigned int);
    unsigned char check_module_run(void);
    unsigned char check_sim_card(void);
    unsigned char check_register(unsigned char);
    unsigned char check_csq(void);
    unsigned char check_module_version(unsigned char type);
#ifdef	__cplusplus
}
#endif

#endif	/* MODULE_LIBRARY_H */

