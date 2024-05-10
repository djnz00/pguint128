/*
 * Faster ntoa than sprintf("%u")
 * - use count leading zeroes (CLZ) intrinsic to determine log2 of value;
 *   this is a single instruction on all modern architectures; the
 *   implementation falls back to C code
 * - convert log2 to log10 using small lookup tables
 * - uses log10 to jump forward and output decimal backwards
 */

/* define likely/unlikely if needed */
#ifdef __GNUC__
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#endif

#ifndef likely
#define likely(x) (x)
#endif
#ifndef unlikely
#define unlikely(x) (x)
#endif

/* CLZ first choice: gcc/clang intrinsic */
#ifdef __GNUC__
#define clz32(v) __builtin_clz(v)
#define clz64(v) __builtin_clzll(v)
#endif /* __GNUC__ */

/* CLZ second choice: MSVC intrinsic */
#ifdef _MSC_VER
#include <intrin.h>
/* caller-assured pre-condition: v != 0 */
static uint32_t
__forceinline clz32_(uint32_t v)
{
    DWORD n;
    _BitScanReverse(&n, v);
	return 31 - n;
}
static uint64_t
__forceinline clz64_(uint64_t v)
{
    DWORD n;
    _BitScanReverse64(&n, v);
	return 63 - n;
}
#define clz32(v) clz32_(v)
#define clz64(v) clz64_(v)
#endif /* _MSC_VER */

/* CLZ third choice: fallback C code */
#ifndef clz32
static uint32_t
clz32_(uint32_t v)
{
    v |= (v >> 1);
    v |= (v >> 2);
    v |= (v >> 4);
    v |= (v >> 8);
    v |= (v >> 16);
    v -= ((v >> 1) & 0x55555555);
    v = (((v >> 2) & 0x33333333) + (v & 0x33333333));
    v = (((v >> 4) + v) & 0x0f0f0f0f);
    v += (v >> 8);
    v += (v >> 16);
    return 32 - (v & 0x3f);
}
#define clz32(v) clz32_(v)
#endif
#ifndef clz64
static uint64_t
clz64_(uint64_t v)
{
    return (v>>32) ? clz32(v>>32) : 32 + clz32(v);
}
#define clz64(v) clz64_(v)
#endif

/*
 * pow10 using LUTs
 */
static uint32_t
pow10_32(unsigned int i)
{
	static uint32_t pow10[] = {
		1U,
		10U,
		100U,
		1000U,
		10000U,
		100000U,
		1000000U,
		10000000U,
		100000000U,
		1000000000U
	};
	return pow10[i];
}

static uint64_t
pow10_64(unsigned int i)
{
	static uint64_t pow10[] = {
		1ULL,
		10ULL,
		100ULL,
		1000ULL,
		10000ULL,
		100000ULL,
		1000000ULL,
		10000000ULL,
		100000000ULL,
		1000000000ULL,
		10000000000ULL,
		100000000000ULL,
		1000000000000ULL,
		10000000000000ULL,
		100000000000000ULL,
		1000000000000000ULL,
		10000000000000000ULL,
		100000000000000000ULL,
		1000000000000000000ULL,
		10000000000000000000ULL
	};
	return pow10[i];
}

/*
 * ntoa using clz, LUT, pow10
 */
static void
utoa8(char *buf, uint8_t v)
{
	unsigned int n;
	if (likely(v < 10))
		n = 1;
	else if (likely(v < 100))
		n = 2;
	else
		n = 3;
	buf[n] = 0;
	do { buf[--n] = (v % 10) + '0'; v /= 10; } while (likely(n));
}

static void
itoa8(char *buf, int8_t v)
{
	if (v < 0) {
		*buf++ = '-';
		v = -v;
	}
	utoa8(buf, v);
}

static void
utoa32(char *buf, uint32_t v)
{
	static uint8_t clz10[] = {
		20, 20, 19, 18, 18, 17, 16, 16, 15, 14,
		14, 14, 13, 12, 12, 11, 10, 10,  9,  8,
		 8,  8,  7,  6,  6,  5,  4,  4,  3,  2,
		 2,  2
	};
	unsigned int n;
	if (unlikely(!v))
		n = 1;
	else {
		unsigned int o = clz10[clz32(v)];
		if (likely(!(o & 1)))
			n = o>>1;
		else {
			o >>= 1;
			n = o + (v >= pow10_32(o));
		}
	}
	buf[n] = 0;
	do { buf[--n] = (v % 10) + '0'; v /= 10; } while (likely(n));
}

static unsigned int
log10_64(uint64_t v)
{
	static uint8_t clz10[] = {
		39, 38, 38, 38, 37, 36, 36, 35, 34, 34,
		33, 32, 32, 32, 31, 30, 30, 29, 28, 28,
		27, 26, 26, 26, 25, 24, 24, 23, 22, 22,
		21, 20, 20, 20, 19, 18, 18, 17, 16, 16,
		15, 14, 14, 14, 13, 12, 12, 11, 10, 10,
		 9,  8,  8,  8,  7,  6,  6,  5,  4,  4,
		 3,  2,  2,  2
	};
	unsigned int n;
	if (unlikely(!v))
		n = 1;
	else {
		unsigned o = clz10[clz64(v)];
		if (likely(!(o & 1)))
			n = o>>1;
		else {
			o >>= 1;
			n = o + (v >= pow10_64(o));
		}
	}
	return n;
}

static void
utoa64(char *buf, uint64_t v)
{
	unsigned n = log10_64(v);
	buf[n] = 0;
	do { buf[--n] = (v % 10) + '0'; v /= 10; } while (likely(n));
}

static unsigned int
log10_128(uint128_t v)
{
     uint128_t f = (uint128_t)10000000000000000000ULL;
     unsigned int n;
	 if (likely(v < f))
		 n = log10_64(v);
     else {
	     v /= f;
		 if (likely(v < f))
		   n = log10_64(v) + 19;
		 else {
		   v /= f;
		   n = log10_64(v) + 38;
		 }
	 }
	return n;
}

static void
utoa128(char *buf, uint128_t v)
{
	unsigned n = log10_128(v);
	buf[n] = 0;
	do { buf[--n] = (v % 10) + '0'; v /= 10; } while (likely(n));
}

static void
itoa128(char *buf, int128_t v)
{
	if (v < 0) {
		*buf++ = '-';
		v = -v;
	}
	utoa128(buf, v);
}
