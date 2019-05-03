#include "initial.h"
#include <pic18f26k22.h>

unsigned char read_int_ee(unsigned char col)
{
	EEADRL = col;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
	return(EEDATL);
}

void write_int_ee(unsigned char col,unsigned char mdata)
{
	EEADRL = col;
	EEDATL = mdata;
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
}

