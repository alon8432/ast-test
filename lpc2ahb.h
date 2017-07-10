#ifndef LPC2AHB_H
#define LPC2AHB_H

#define LPC2AHB_LDN 0x0D

typedef  struct RegAndOrStruct
{
	unsigned char ldn;
	unsigned char reg;
	unsigned char and_data;
	unsigned char or_data;
	unsigned char data;//record current data
}RegAndOr;

int table_modify(RegAndOr *dest, int count, unsigned char ldn, unsigned char reg, unsigned char and_data, unsigned char or_data, unsigned char data);
int table_operate(RegAndOr *p, int count, int write_format);
int get_data(RegAndOr *dest, int count, unsigned char ldn, unsigned *pdata);

#endif //LPC2AHB_H
