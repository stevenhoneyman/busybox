/* vi: set sw=4 ts=4: */
/*
 * Mini nproc implementation for busybox
 *
 * Copyright (C) 2014 by Steven Honeyman <stevenhoneyman@gmail.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

//usage:#define nproc_trivial_usage
//usage:	""
//usage:#define nproc_full_usage "\n\n"
//usage:	"Print the number of available processing units"

//config:config NPROC
//config:	bool "nproc"
//config:	default y
//config:	help
//config:	  Print the number of available processing units.

//kbuild:lib-$(CONFIG_NPROC) += nproc.o
//applet:IF_NPROC(APPLET_NOFORK(nproc, nproc, BB_DIR_USR_BIN, BB_SUID_DROP, nproc))

#include "libbb.h"
/* no options, no getopt */

/* This is a NOFORK applet. Be very careful! */

int nproc_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int nproc_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	int nprocs;

	if (argv[1])
		bb_show_usage();

	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (nprocs > 0) {
		puts(itoa(nprocs));
	} else {
		bb_perror_nomsg_and_die();
	}

	return fflush_all();
}
