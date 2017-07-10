#include "lpc2ahb.h"
#include "io.h"
#include <stdio.h>
#include <unistd.h>

#define DEBUG_PRINT 0x00

RegAndOr ahb_bus_read_tab[]=
{
	//unlock
	//active
	{LPC2AHB_LDN,0xF0,0x00,0x00,0xFF},//write address
	{LPC2AHB_LDN,0xF1,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF2,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF3,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF8,0xFC,0x02,0x00},//Write Mode
	{LPC2AHB_LDN,0xFE,0xFF,0x00,0x00},//Fire Read
	{LPC2AHB_LDN,0xF4,0xFF,0x00,0xFF},//Read data
	{LPC2AHB_LDN,0xF5,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xF6,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xF7,0xFF,0x00,0xFF},
	//lock
};
const int ahb_bus_read_tab_count =sizeof(ahb_bus_read_tab)/sizeof(RegAndOr);

RegAndOr ahb_bus_write_tab[]=
{
	//unlock
	//active
	{LPC2AHB_LDN,0xF0,0x00,0x00,0xFF},//write address
	{LPC2AHB_LDN,0xF1,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF2,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF3,0x00,0x00,0xFF},
	{LPC2AHB_LDN,0xF8,0xFC,0x02,0x00},//Write Mode
	{LPC2AHB_LDN,0xF4,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xF5,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xF6,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xF7,0xFF,0x00,0xFF},
	{LPC2AHB_LDN,0xFE,0x00,0xCF,0xFF},//Fire wrtie
	//lock
};
const int ahb_bus_write_tab_count=sizeof(ahb_bus_write_tab)/sizeof(RegAndOr);

RegAndOr skip_sync_tab[]=
{
	{LPC2AHB_LDN,0xF8,0xFF,0x00,0xFF},//write gpio address
	{LPC2AHB_LDN,0xFE,0xFF,0x00,0xFF},
};
const int skip_sync_tab_count=sizeof(skip_sync_tab)/sizeof(RegAndOr);

static void unlock()
{
	byte_write(0xa5, INDEX_PORT);
	byte_write(0xa5, INDEX_PORT);
}

static void lock()
{
	byte_write(0xaa, INDEX_PORT);
}

int enable_lpc2ahb()
{
	isa_write(0x30, 0x01);
}

int table_modify(RegAndOr *dest, int count, unsigned char ldn, unsigned char reg, unsigned char and_data, unsigned char or_data, unsigned char data)
{
	int n = 0;
	if(!dest) return -1;
	for(n=0; n<count; n++)
	{
		if((dest[n].ldn == ldn) && (dest[n].reg == reg))
		{
			dest[n].and_data = and_data;
			dest[n].or_data=or_data;
			dest[n].data=data;
			if(IO_DEBUG) printf("table_modify, reg[%02x], and_data = %02x, or_data = %02x, data = %02x \n", dest[n].reg, and_data, or_data, data);
		}
	}
	return 0;
}

/* 
write_format
1: write
0: read
*/
int table_operate(RegAndOr *p, int count, int write_format)
{
	if(!p) return -1;
	unlock();
	enable_lpc2ahb();
	if(IO_DEBUG) printf("lpc2ahb enabled = %x \n", isa_read(0x30));

	isa_write(LDN_REG, p[0].ldn);

	for(int n=0;n<count;n++)
	{
		if(!(p[n].and_data == 0xff && p[n].or_data == 0x00)) //not only read
		{
			p[n].data &= p[n].and_data;
			p[n].data |= p[n].or_data;
			isa_write(p[n].reg, p[n].data);
		}
		if(write_format == 0)
		{
			if(p[n].reg == 0xfe) isa_read(p[n].reg);
			if(p[n].reg >= 0xf4) p[n].data = isa_read(p[n].reg);
		}
		
		if(IO_DEBUG) printf("table_operate, reg[%02x] = %02x \n", p[n].reg, p[n].data);
	}
	lock();
	return 0;
}

int get_data(RegAndOr *dest, int count, unsigned char ldn, unsigned *pdata)
{
	int n = 0;
	unsigned char reg = 0xf4;
	unsigned temp_data;

	if(!(dest||pdata)) return -1;
	for(n=0;n<count;n++)
	{
		if(dest[n].ldn==ldn&&dest[n].reg==reg)
		{
			break;
		}
	}
/*
	pdata[0] = dest[n].data;
	pdata[1] = dest[n+1].data;
	pdata[2] = dest[n+2].data;
	pdata[3] = dest[n+3].data;
*/
	temp_data = dest[n].data << 24 | dest[n+1].data << 16 | dest[n+2].data << 8 | dest[n+3].data;
	if(IO_DEBUG) printf("%lx \n", temp_data);

	*pdata = temp_data;

	if(IO_DEBUG) printf("%lx \n", *pdata);
	//printf("%02x %02x %02x %02x \n", pdata[0], pdata[1], pdata[2], pdata[3]);
	return 0;
}
