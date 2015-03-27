/* vi: set sw=4 ts=4: */
/*
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#include "libbb.h"

char* FAST_FUNC is_prefixed_with(const char *string, const char *key)
{
#if 0	/* Two passes over key - probably slower */
	int len = strlen(key);
	if (strncmp(string, key, len) == 0)
		return string + len;
	return NULL;
#else	/* Open-coded */
	while (*key != '\0') {
		if (*key != *string)
			return NULL;
		key++;
		string++;
	}
	return (char*)string;
#endif
}

/* returns the array index of the string */
/* (index of first match is returned, or -1) */
int FAST_FUNC index_in_str_array(const char *const string_array[], const char *key)
{
	int i;

	for (i = 0; string_array[i] != 0; i++) {
		if (strcmp(string_array[i], key) == 0) {
			return i;
		}
	}
	return -1;
}

int FAST_FUNC index_in_strings(const char *strings, const char *key)
{
	int idx = 0;

	while (*strings) {
		if (strcmp(strings, key) == 0) {
			return idx;
		}
		strings += strlen(strings) + 1; /* skip NUL */
		idx++;
	}
	return -1;
}

/* returns the array index of the string, even if it matches only a beginning */
/* (index of first match is returned, or -1) */
#ifdef UNUSED
int FAST_FUNC index_in_substr_array(const char *const string_array[], const char *key)
{
	int i;
	if (key[0]) {
		for (i = 0; string_array[i] != 0; i++) {
			if (is_prefixed_with(string_array[i], key)) {
				return i;
			}
		}
	}
	return -1;
}
#endif

int FAST_FUNC index_in_substrings(const char *strings, const char *key)
{
	int matched_idx = -1;
	const int len = strlen(key);

	if (len) {
		int idx = 0;
		while (*strings) {
			if (strncmp(strings, key, len) == 0) {
				if (strings[len] == '\0')
					return idx; /* exact match */
				if (matched_idx >= 0)
					return -1; /* ambiguous match */
				matched_idx = idx;
			}
			strings += strlen(strings) + 1; /* skip NUL */
			idx++;
		}
	}
	return matched_idx;
}

const char* FAST_FUNC nth_string(const char *strings, int n)
{
	while (n) {
		n--;
		strings += strlen(strings) + 1;
	}
	return strings;
}

#ifdef UNUSED_SO_FAR /* only brctl.c needs it yet */
/* Returns 0 for no, 1 for yes or a negative value on error.  */
smallint FAST_FUNC yesno(const char *str)
{
	static const char no_yes[] ALIGN1 =
		"0\0" "off\0" "no\0"
		"1\0" "on\0" "yes\0";
	int ret = index_in_substrings(no_yes, str);
	return ret / 3;
}
#endif
