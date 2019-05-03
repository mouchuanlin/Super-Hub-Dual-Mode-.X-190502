#include "initial.h"
#include <pic18f26k22.h>
#include <xc.h>

unsigned char read_ee(unsigned char page,unsigned char addr)
{
    unsigned char p_data;
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
    p_data = EEDATA;
    GIE = 1;
    CLRWDT();
	return(p_data);
}

void write_ee(unsigned char page,unsigned char addr,unsigned char data_p)
{
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	EEDATA = data_p;
	CFGS = 0;
	EEPGD = 0;
	WREN = 1;
	GIE = 0;
	EECON2 = 0x55;
	EECON2 = 0xaa;
	WR = 1;
	GIE = 1;
	WREN = 0;
	while( WR==1 );
    CLRWDT();
    GIE = 1;
}
