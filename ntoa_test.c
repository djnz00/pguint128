#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

typedef __int128_t int128_t;
typedef __uint128_t uint128_t;

#include "ntoa.h"

/*
 * ntoa.h test program
 */

void
testi8()
{
	int8_t i;
	unsigned int n;
	char buf[8];

	i = -128;
	do {
		itoa8(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 4);
		assert(i < 0 ? (buf[1] != '0') : (!i || buf[0] != '0'));
		assert(strtol(buf, 0, 10) == i);
		++i;
	} while (i != -128);	/* wraparound */
	printf("int8_t final value %s\n", buf);
}

void
testu8()
{
	uint8_t i;
	unsigned int n;
	char buf[8];

	i = 0;
	do {
		utoa8(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 3);
		assert(!i || buf[0] != '0');
		assert(strtoul(buf, 0, 10) == i);
		++i;
	} while (i);	/* wraparound */
	printf("uint8_t final value %s\n", buf);
}

void
testu32()
{
	uint32_t i;
	unsigned int n;
	char buf[16];

	i = 0;
	do {
		utoa32(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 10);
		assert(!i || buf[0] != '0');
		assert(strtoul(buf, 0, 10) == i);
		i += 64;
	} while (i);	/* wraparound */
	printf("uint32_t final value %s\n", buf);
}

void
testu64()
{
	uint64_t i;
	unsigned int n;
	char buf[24];

	i = 0;
	do {
		utoa64(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 20);
		assert(!i || buf[0] != '0');
		assert(strtoull(buf, 0, 10) == i);
		i += (((uint64_t)64)<<32);
	} while (i);	/* wraparound */
	printf("uint64_t final value %s\n", buf);
}

int
main()
{
	testi8();
	testu8();
	testu32();
	testu64();
	puts("all tests passed");
	return 0;
}
