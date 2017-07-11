#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "io.h"
#include "lpc2ahb.h"

extern RegAndOr ahb_bus_read_tab[];
extern const int ahb_bus_read_tab_count;
extern RegAndOr ahb_bus_write_tab[];
extern const int ahb_bus_write_tab_count;

#define MUMBER_ARGC                 4
#define MAX_ARGC_FOR_READ			4
#define MAX_ARGC_FOR_WRITE			6

#define _STRING_LEN_256_    256
#define _STRING_LEN_128_    128
#define _STRING_LEN_4_      "4"
#define _STRING_LEN_10_     10
#define _STRING_LEN_16_     16

static const struct option long_options[] =
{
   { "read",                no_argument,       0,    0   },
   { "write",               no_argument,       0,    0   },
   { "address",             required_argument, 0,    0   },
   { "data",                required_argument, 0,    0   },
   { "help",                no_argument,       0,    0   },
   { 0,                     0,                 0,    0   }
};

static const char *optstring = "a:d:hrw";
static void show_usage()
{
	const char * const usage_text = "\
	Usage :	regmemapp -[r|w] -a -l -d \n\
	Options:\n\
		-r|w,	mode of operation to read/write \n\
		-a,	address to read/write ( address in hex format )\n\
		-d,	data to write (data in hex format )\n\
		-h,	print usage\n\
	e.g:\n\
	Default read ( read format - hex )\n\
		ast_test -r -a 0x1e6e2090 \n\
	write operation\n\
		ast_test -w -a 0x1e6e2090 -d 0x0fffa003\n\
	\n";

	fprintf(stderr,"%s", usage_text);
}

int main (int argc , char* argv [])
{
	int opt = 0, length_ascii = 0;
    char read_format = 0, write_format = 0, help_format = 0, address_format = 0;
	char *address = NULL , *data = NULL, *length = NULL;
	unsigned long address_long;
	unsigned u32_data;

	/*Check minimum arguments available*/
    if ( argc < MUMBER_ARGC )
    {
        show_usage ();
        return 0;
    }

    /* parse command line data with arguments
       -r -w - read / write operation no arguments
       -l -d - length / data with arguments
       -h    - help no arguments
    */
    while ( (opt = getopt_long ( argc, argv, optstring , long_options, 0 )) != -1)
    {
        if ( opt == '?' )
        {
            printf ( "Invalid arguments\n" );
            show_usage();
            return 0;
        }
        switch ( opt )
        {
            case 'r':
                read_format = opt;
                break;
            case 'w':
                write_format = opt;
                break;
            case 'h':
                help_format = opt;
                break;
            case 'a':
                address_format = opt;
                address = optarg;
                break;
            case 'd':
                data = optarg;
                break;
            case '?':
            default :
                show_usage();
                return 0;
        }
    }
    if ( read_format == 'r' )
    {
    	// to read non-range of address ( read by address and length  )
        if ( address_format == 'a' )
        {
        	/*   for read operation
                 address & length - required
             */
        	if ( address == NULL)
            {
                printf ( "Address/Length required for read operation \n" );
                show_usage();
                return 0;
            }
            /*
               Maximum argument = 4  ---> /usr/local/bin/regmemapp -a 0x1e6e2090 -r
               read format with help format & write format are not valid
             */
            if ( argc > MAX_ARGC_FOR_READ || help_format == 'h' || write_format == 'w' )
            {
                printf ( "Invalid arguments\n" );
                show_usage();
                return 0;
            }
            address_long = strtoul ( address, NULL, _STRING_LEN_16_ );
            if(IO_DEBUG) printf("address_long = %x \n", address_long);

            /* unlock SCU */
            if((address_long && 0xfffff000) == 0x1e6e2000)
            {
            	u32_data = 0x1688A8A8;
            	table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf7, 0x00, (unsigned char)(u32_data), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf6, 0x00, (unsigned char)(u32_data>>8), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf5, 0x00, (unsigned char)(u32_data>>16), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf4, 0x00, (unsigned char)(u32_data>>24), 0x00);

				get_privilege_level();
				table_operate(ahb_bus_write_tab,ahb_bus_write_tab_count, 1);
				release_privilege_level();
            }

            table_modify(ahb_bus_read_tab, ahb_bus_read_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
			table_modify(ahb_bus_read_tab, ahb_bus_read_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
			table_modify(ahb_bus_read_tab, ahb_bus_read_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
			table_modify(ahb_bus_read_tab, ahb_bus_read_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);

			get_privilege_level();
			table_operate(ahb_bus_read_tab,ahb_bus_read_tab_count, 0);
		//	io_list();
			release_privilege_level();

			get_data(ahb_bus_read_tab,ahb_bus_read_tab_count,LPC2AHB_LDN,&u32_data);

			printf("%lx: %08x \n", address_long, u32_data);

			/* lock SCU again*/
            if((address_long && 0xfffff000) == 0x1e6e2000)
            {
            	u32_data = 0;
            	table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf7, 0x00, (unsigned char)(u32_data), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf6, 0x00, (unsigned char)(u32_data>>8), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf5, 0x00, (unsigned char)(u32_data>>16), 0x00);
				table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf4, 0x00, (unsigned char)(u32_data>>24), 0x00);

				get_privilege_level();
				table_operate(ahb_bus_write_tab,ahb_bus_write_tab_count, 1);
				release_privilege_level();
            }
        }
    }
	// Write operation
    else if ( write_format == 'w' )
    {
    	/*
            write operation should have address, length & data
            ast_test -w -a 0x1e6e2090 -d 0x0fffa003
        */
        if ( address == NULL || data == NULL  )
        {
            if(IO_DEBUG) printf( " write_format = %c  address = %s data = %s length = %s\n", write_format, address, data, length );
            printf ( "Address/Length/Data required\n" );
            show_usage();
            return 0;
        }
        /*
            Write operation must have 6 arguments ---> ast_test -w -a 0x1e6e2090 -d 0x0fffa003
            write format should not have help / read format
        */
        if ( argc != MAX_ARGC_FOR_WRITE || help_format == 'h' || read_format == 'r' )
        {
            printf ( "Invalid arguments\n" );
            show_usage();
            return 0;
        }
        if(IO_DEBUG) printf( " write_format = %c  address = %s data = %s \n", write_format, address, data );
        address_long = strtoul ( address, NULL, _STRING_LEN_16_ );
        u32_data = strtoul ( data, NULL, _STRING_LEN_16_ );
        if(IO_DEBUG) printf("address_long = %x, u32_data = %x \n", address_long, u32_data);

        /* unlock SCU */
        if((address_long && 0xfffff000) == 0x1e6e2000)
        {
            u32_data = 0x1688A8A8;
            table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf7, 0x00, (unsigned char)(u32_data), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf6, 0x00, (unsigned char)(u32_data>>8), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf5, 0x00, (unsigned char)(u32_data>>16), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf4, 0x00, (unsigned char)(u32_data>>24), 0x00);

			get_privilege_level();
			table_operate(ahb_bus_write_tab,ahb_bus_write_tab_count, 1);
			release_privilege_level();
        }

        table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf7, 0x00, (unsigned char)(u32_data), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf6, 0x00, (unsigned char)(u32_data>>8), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf5, 0x00, (unsigned char)(u32_data>>16), 0x00);
		table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf4, 0x00, (unsigned char)(u32_data>>24), 0x00);

		get_privilege_level();
		table_operate(ahb_bus_write_tab,ahb_bus_write_tab_count, 1);
		release_privilege_level();

		/* unlock SCU */
        if((address_long && 0xfffff000) == 0x1e6e2000)
        {
            u32_data = 0;
            table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf3, 0x00, (unsigned char)(address_long), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf2, 0x00, (unsigned char)(address_long>>8), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf1, 0x00, (unsigned char)(address_long>>16), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf0, 0x00, (unsigned char)(address_long>>24), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf7, 0x00, (unsigned char)(u32_data), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf6, 0x00, (unsigned char)(u32_data>>8), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf5, 0x00, (unsigned char)(u32_data>>16), 0x00);
			table_modify(ahb_bus_write_tab, ahb_bus_write_tab_count, LPC2AHB_LDN, 0xf4, 0x00, (unsigned char)(u32_data>>24), 0x00);

			get_privilege_level();
			table_operate(ahb_bus_write_tab,ahb_bus_write_tab_count, 1);
			release_privilege_level();
        }
    }

	//printf("data read = %x \n", u32_data);
	return 0;
}
