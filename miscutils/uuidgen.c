/* vi: set sw=4 ts=4: */
/*
 * Mini uuidgen implementation for busybox
 *
 * Copyright (C) 2014 by Steven Honeyman <stevenhoneyman@gmail.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

//usage:#define uuidgen_trivial_usage
//usage:	""
//usage:#define uuidgen_full_usage "\n\n"
//usage:	"Create a new UUID value"

//config:config UUIDGEN
//config:	bool "uuidgen"
//config:	default n
//config:	help
//config:	  Create a new UUID value

//kbuild:lib-$(CONFIG_UUIDGEN) += uuidgen.o
//applet:IF_UUIDGEN(APPLET(uuidgen, BB_DIR_USR_BIN, BB_SUID_DROP))

#include "libbb.h"
/* no options, no getopt */

int uuidgen_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int uuidgen_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	uint8_t uuid[16];
	char uuid_string[32];

	if (argv[1])
		bb_show_usage();

	generate_uuid(uuid);
	bin2hex(uuid_string, (char *)uuid, 16);

	/* borrowed from mkswap.c */
	printf("%.8s"  "-%.4s-%.4s-%.4s-%.12s\n",
		uuid_string,
		uuid_string+8,
		uuid_string+8+4,
		uuid_string+8+4+4,
		uuid_string+8+4+4+4
	);

	return fflush_all();
}
