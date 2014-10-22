/* vi: set sw=4 ts=4: */
/*
 * bin2c implementation for busybox
 *
 * Mostly stolen from the Linux kernel source tree
 * (bin2c.c 1999 Matt Mackall <mpm@selenic.com>)
 *
 * Copyright (C) 2014 by Steven Honeyman <stevenhoneyman@gmail.com>
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

//usage:#define bin2c_trivial_usage
//usage:	"<[FILE] [NAME]"
//usage:#define bin2c_full_usage "\n\n"
//usage:	"Convert binary data from STDIN to a C include"

//config:config BIN2C
//config:	bool "bin2c"
//config:	default n
//config:	help
//config:	  Convert binary data from STDIN to a C include

//kbuild:lib-$(CONFIG_BIN2C) += bin2c.o
//applet:IF_BIN2C(APPLET(bin2c, BB_DIR_USR_BIN, BB_SUID_DROP))

#include "libbb.h"

int bin2c_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int bin2c_main(int argc UNUSED_PARAM, char **argv)
{
    int ch, total=0;

    if (argv[1]==NULL)
	bb_show_usage();

    printf("const char %s[] =\n", argv[1]);

    do {
	printf("\t\"");
	while ((ch = getchar()) != EOF) {
	    total++;
	    printf("\\x%02x",ch);
	    if (total % 16 == 0) break;
	}
	printf("\"\n");
    } while (ch != EOF);

    printf("\t;\n\nconst int %s_size = %d;\n", argv[1], total);

    return fflush_all();
}
