#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

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

static unsigned int
atou128(const char *s, uint128_t *r)
{
    int c = s[0];
	uint128_t v;
	unsigned int o;
	if (unlikely(c < '0' || c > '9')) return 0;
	v = c - '0';
	o = 1;
	while (likely(o < 39 && (c = s[o]) >= '0' && c <= '9')) {
		v = v * 10 + (c - '0');
		++o;
	}
	*r = v;
	return o;
}

static unsigned int
atoi128(const char *s, int128_t *r)
{
	if (s[0] == '-') {
		unsigned int o = atou128(&s[1], (uint128_t *)r);
		if (!o) return 0;
		*r = -*r;
		return o + 1;
	}
	return atou128(s, (uint128_t *)r);
}

/* both gcc and clang have a 128bit wraparound bug, so avoid that below */

void
testi128()
{
	int128_t i;
	unsigned int n;
	char buf[48];

	i = ((uint128_t)1)<<127;
	do {
		itoa128(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 40);
		assert(!i || buf[0] != '0');
		int128_t j;
		assert(atoi128(buf, &j) == n);
		assert(i == j);
		i += (((int128_t)0x00000100)<<96);
	} while (i != (((int128_t)0x7fffff00)<<96));
	printf("int128_t final value %s\n", buf);
}

void
testu128()
{
	uint128_t i;
	unsigned int n;
	char buf[48];

	i = 0;
	do {
		utoa128(buf, i);
		n = strlen(buf);
		assert(n > 0 && n <= 40);
		assert(!i || buf[0] != '0');
		uint128_t j;
		assert(atou128(buf, &j) == n);
		assert(i == j);
		i += (((uint128_t)0x00000100)<<96);
	} while (i != (((uint128_t)0xffffff00)<<96));
	printf("uint128_t final value %s\n", buf);
}

int
main()
{
	testi8();
	testu8();
	testu32();
	testu64();
	testi128();
	testu128();
	puts("all tests passed");
	return 0;
}
