#ifndef IO_H
#define IO_H

#ifndef IO_DEBUG
#define IO_DEBUG 0
#endif

typedef unsigned char INT8U;
typedef unsigned short INT16U;
typedef unsigned INT32U;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define INDEX_PORT	0x2E  //change 4E,4F  if  H/W design
#define DATA_PORT	0x2F
#define LDN_REG		0x07

void get_privilege_level();
void release_privilege_level();

void byte_write(unsigned char byte, unsigned port);
unsigned byte_read(unsigned port);
unsigned isa_read(unsigned char index);
void isa_write(unsigned char index,unsigned char data);
void io_list();

#endif // IO_H
