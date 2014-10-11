/* acpi.c - show power state
 *
 * Copyright 2013-2014 A.D., Isaac Dunham
 *
 * Based on toybox 0.5.0+ acpi.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

//config:config ACPI
//config:	bool "acpi"
//config:	default y
//config:	select PLATFORM_LINUX
//config:	help
//config:	  Show status of power sources and thermal devices.

//applet:IF_ACPI(APPLET(acpi, BB_DIR_USR_BIN, BB_SUID_DROP))
//kbuild:lib-$(CONFIG_ACPI) += acpi.o

//usage:#define acpi_trivial_usage
//usage:    "[-abctV]"
//usage:#define acpi_full_usage "\n\n"
//usage:    "Show status of power sources and thermal devices.\n"
//usage:    "-a	show power adapters\n"
//usage:    "-b	show batteries\n"
//usage:    "-c	show cooling device state\n"
//usage:    "-t	show temperatures\n"
//usage:    "-V	show everything"


#define FLAG_a	1
#define FLAG_b	2
#define FLAG_c	4
#define FLAG_t	8
#define FLAG_V	16

#include "libbb.h"
#include <glob.h>

#define G_BUFSIZ 64
struct globals {
    int ac;
    int bat;
    int therm;
    int cool;
    char buf[G_BUFSIZ];
} FIX_ALIASING;
#define G (*(struct globals*)&bb_common_bufsiz1)

static int read_int_at(int dirfd, const char *name)
{
    int fd, ret = 0;
    FILE *fil;

    if ((fd = openat(dirfd, name, O_RDONLY)) < 0)
	return -1;
    fscanf(fil = fdopen(fd, "r"), "%d", &ret);
    fclose(fil);

    return ret;
}

static char *read_type_at(int dirfd)
{
    int fd;
    if ((fd = openat(dirfd, "type", O_RDONLY)) < 0)
	return 0;
    read(fd, G.buf, G_BUFSIZ-1);
    close(fd);
    if errno
	return 0;
    return G.buf;
}

int acpi_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int acpi_main(int argc UNUSED_PARAM, char **argv)
{
    glob_t files;
    int dfd, on, cur, max, temp, i;
    unsigned opts = getopt32(argv, "abctV");

    if (opts & FLAG_V)
	opts = FLAG_a | FLAG_b | FLAG_c | FLAG_t;
    if (!opts)
	opts = FLAG_b;
    if (opts & (FLAG_a | FLAG_b)) {
	glob("/sys/class/power_supply/*/", GLOB_NOSORT, NULL, &files);
	for (i = 0; i < files.gl_pathc; i++) {
	    errno = 0;
	    if (0 <= (dfd = open(files.gl_pathv[i], O_RDONLY))) {
		if (!read_type_at(dfd))
		    goto ab_done;

		if ((!strncmp(G.buf, "Battery", 7)) && (opts & FLAG_b)) {
		    int cap = read_int_at(dfd, "capacity");

		    if (cap < 0) {
			if ((max = read_int_at(dfd, "charge_full")) > 0)
			    cur = read_int_at(dfd, "charge_now");
			else if ((max =
				  read_int_at(dfd, "energy_full")) > 0)
			    cur = read_int_at(dfd, "energy_now");
			if (max > 0)
			    cap = 100 * cur / max;
		    }
		    if (cap >= 0)
			printf("Battery %d: %d%%\n", G.bat++, cap);

		} else if ((opts & FLAG_a)
			   && ((on = read_int_at(dfd, "online")) >= 0)) {
		    printf("Adapter %d: %s-line\n", G.ac++,
			   (on ? "on" : "off"));
		}
	      ab_done:
		close(dfd);
	    }
	}
	globfree(&files);
    }
    if (opts & FLAG_t) {
	glob("/sys/class/*/*/temp", GLOB_NOSORT, NULL, &files);
	glob("/sys/class/*/*/temp*_input", GLOB_NOSORT | GLOB_APPEND, NULL,
	     &files);
	for (i = 0; i < files.gl_pathc; i++) {
	    errno = 0;
	    if ((0 < (temp = read_int_at(AT_FDCWD, files.gl_pathv[i])))
		|| !errno) {
		//some tempertures are in milli-C, some in deci-C
		//reputedly some are in deci-K, but I have not seen them
		if (((temp >= 1000) || (temp <= -1000))
		    && (temp % 100 == 0))
		    temp /= 100;
		printf("Thermal %d: %d.%d degrees C\n", G.therm++,
		       temp / 10, temp % 10);
	    }
	}
	globfree(&files);
    }
    if (opts & FLAG_c) {
	glob("/sys/class/thermal/*/", GLOB_NOSORT, NULL, &files);
	for (i = 0; i < files.gl_pathc; i++) {
	    errno = 0;
	    memset(G.buf, 0, 64);
	    if (0 <= (dfd = open(files.gl_pathv[i], O_RDONLY))) {
		if (read_type_at(dfd) && !errno) {
		    G.buf[strlen(G.buf) - 1] = 0;
		    cur = read_int_at(dfd, "cur_state");
		    max = read_int_at(dfd, "max_state");
		    if (errno)
			printf("Cooling %d: %s no state information\n",
			       G.cool++, G.buf);
		    else
			printf("Cooling %d: %s %d of %d\n", G.cool++, G.buf,
			       cur, max);
		}
		close(dfd);
	    }
	}
	globfree(&files);
    }
    return 0;
}
