#ifndef  WINDOWS_OS
#define LINUX_OS 1
#endif

#if LINUX_OS
#include <sys/io.h>
#endif

#include "io.h"
#include <stdio.h>

void get_privilege_level()
{
#if LINUX_OS
	iopl(3);	//Open Access IO right
#endif
}

void release_privilege_level()
{
#if LINUX_OS
        iopl(0);        //Close Access IO right
#endif
}

void byte_write(unsigned char byte, unsigned port)
{
#if LINUX_OS
	 outb(byte,port);
#endif
}

unsigned byte_read(unsigned port)
{
#if LINUX_OS
	return inb(port);
#endif
	return 0;
}

unsigned isa_read(unsigned char index)
{
	byte_write(index, INDEX_PORT);
	return byte_read(DATA_PORT);
}

void isa_write(unsigned char index, unsigned char data)
{
	byte_write(index, INDEX_PORT);
	byte_write(data, DATA_PORT);
}

void io_list()
{
	unsigned short io_addr=0x20;	
	unsigned char data;

	for(int n=0;n<0xFFFF;n++)
	{
		data=byte_read(io_addr+n);
		if(data!=0xFF&&data!=00) printf("io_addr[%04X]=%02X\n",io_addr+n,data);
	}	
}
