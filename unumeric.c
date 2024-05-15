#include <postgres.h>
#include <fmgr.h>
#include <utils/fmgrprotos.h>
#include <utils/memutils.h>

#include "unumeric.h"

/* #define UNDEBUG */

#ifdef UNDEBUG
#include <stdio.h>
#include "ntoa.h"
#endif

#ifdef UNDEBUG
static void
numeric_log_(char *fmt, Numeric n)
{
	char *s = DatumGetCString(DirectFunctionCall1(
		numeric_out, NumericGetDatum(n)));
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
	pfree(s);
}

static void
int128_log_(char *fmt, __int128_t u)
{
	char s[40];
	itoa128(s, u);
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
}

static void
uint128_log_(char *fmt, __uint128_t u)
{
	char s[40];
	utoa128(s, u);
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
}
#endif

/* PG numeric division is lossy, so we ensure accuracy by multiplying
 * by reciprocals rather than dividing */

/* cached values of various powers of 2 and their reciprocals */
static Numeric zero = 0;		/* 0 */
static Numeric bit1 = 0;		/* 2 */
static Numeric bit1_ = 0;		/* 1/2 */
static Numeric bit2 = 0;		/* 4 */
static Numeric bit2_ = 0;		/* 1/4 */
static Numeric bit63 = 0;		/* 2^63 */
static Numeric bit63_ = 0;		/* 1/(2^63)*/
static Numeric bit64 = 0;		/* 2^64*/

/* this code depends on the postgres server being single-threaded,
 * or at any rate no overlapping concurrent calls to uint_init_() */
static void uint_init_()
{
	MemoryContext old;

	if (likely(zero)) return;

	/* these cached values need to endure */
	old = MemoryContextSwitchTo(CacheMemoryContext);

	zero = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		"0")));
	bit1 = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		"2")));
	bit1_ = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		".5")));
	bit2 = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		"4")));
	bit2_ = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		".25")));
	bit63 = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		"9223372036854775808")));
	bit63_ = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		".000000000000000000108420217248550443400745280086994171142578125")));
	bit64 = DatumGetNumeric(DirectFunctionCall1(numeric_in, CStringGetDatum(
		"18446744073709551616")));

	MemoryContextSwitchTo(old);

#ifdef UNDEBUG
	numeric_log_("uint_init_() zero=%s", zero);
	numeric_log_("uint_init_() bit1=%s", bit1);
	numeric_log_("uint_init_() bit1_=%s", bit1_);
	numeric_log_("uint_init_() bit2=%s", bit2);
	numeric_log_("uint_init_() bit2_=%s", bit2_);
	numeric_log_("uint_init_() bit63=%s", bit63);
	numeric_log_("uint_init_() bit63_=%s", bit63_);
	numeric_log_("uint_init_() bit64=%s", bit64);
#endif
}

PG_FUNCTION_INFO_V1(uint_init);
Datum
uint_init(PG_FUNCTION_ARGS)
{
	uint_init_();
	PG_RETURN_VOID();
}

int64_t
numeric_to_int64(Numeric n)
{
	return DatumGetInt64(DirectFunctionCall1(
		numeric_int8, NumericGetDatum(n)));
}

static Numeric
numeric_floor_(Numeric v)
{
	return DatumGetNumeric(DirectFunctionCall1(
		numeric_floor, NumericGetDatum(v)));
}

static Numeric
numeric_uminus_(Numeric v)
{
	return DatumGetNumeric(DirectFunctionCall1(
		numeric_uminus, NumericGetDatum(v)));
}

static bool
numeric_lt_(Numeric l, Numeric r)
{
	return DatumGetBool(DirectFunctionCall2(
		numeric_lt, NumericGetDatum(l), NumericGetDatum(r)));
}

uint64_t
numeric_to_uint64(Numeric n)
{
	uint_init_();
#ifdef UNDEBUG
	numeric_log_("numeric_to_uint64(%s)", n);
#endif
	if (unlikely(!numeric_lt_(n, bit63))) {
		Numeric high_down = numeric_mul_opt_error(n, bit1_, NULL);
		Numeric high_floor = numeric_floor_(high_down);
		Numeric high_up = numeric_mul_opt_error(high_floor, bit1, NULL);
		Numeric low = numeric_sub_opt_error(n, high_up, NULL);
		uint64_t v =
			(((uint64_t)numeric_to_int64(high_floor))<<1) |
			numeric_to_int64(low);
#ifdef UNDEBUG
		numeric_log_("numeric_to_uint64() high_down=%s", high_down);
		numeric_log_("numeric_to_uint64() high_floor=%s", high_floor);
		numeric_log_("numeric_to_uint64() high_up=%s", high_up);
		numeric_log_("numeric_to_uint64() low=%s", low);
#endif
		pfree(high_down);
		pfree(high_floor);
		pfree(high_up);
		pfree(low);
		return v;
	}
	return numeric_to_int64(n);
}

__uint128_t
numeric_to_uint128(Numeric n)
{
	uint_init_();
#ifdef UNDEBUG
	numeric_log_("numeric_to_uint128(%s)", n);
	numeric_log_("numeric_to_uint128() bit63=%s", bit63);
#endif
	if (unlikely(!numeric_lt_(n, bit63))) {
		Numeric high_down = numeric_mul_opt_error(n, bit63_, NULL);
		Numeric high_floor = numeric_floor_(high_down);
		Numeric high_up = numeric_mul_opt_error(high_floor, bit63, NULL);
		Numeric low = numeric_sub_opt_error(n, high_up, NULL);
		__uint128_t v;
#ifdef UNDEBUG
		numeric_log_("numeric_to_uint128() high_down=%s", high_down);
		numeric_log_("numeric_to_uint128() high_floor=%s", high_floor);
		numeric_log_("numeric_to_uint128() high_up=%s", high_up);
		numeric_log_("numeric_to_uint128() low=%s", low);
#endif
		if (unlikely(!numeric_lt_(high_floor, bit63))) {
			Numeric hhigh_down = numeric_mul_opt_error(high_floor, bit2_, NULL);
			Numeric hhigh_floor = numeric_floor_(hhigh_down);
			Numeric hhigh_up = numeric_mul_opt_error(hhigh_floor, bit2, NULL);
			Numeric hlow = numeric_sub_opt_error(high_floor, hhigh_up, NULL);
#ifdef UNDEBUG
			numeric_log_("numeric_to_uint128() hhigh_down=%s", hhigh_down);
			numeric_log_("numeric_to_uint128() hhigh_floor=%s", hhigh_floor);
			numeric_log_("numeric_to_uint128() hhigh_up=%s", hhigh_up);
			numeric_log_("numeric_to_uint128() hlow=%s", hlow);
#endif
			v = (((__uint128_t)numeric_to_int64(hhigh_floor))<<65) |
				(((__uint128_t)numeric_to_int64(hlow))<<63) |
				numeric_to_int64(low);
			pfree(hhigh_down);
			pfree(hhigh_floor);
			pfree(hhigh_up);
			pfree(hlow);
		} else {
			v = (((__uint128_t)numeric_to_int64(high_floor))<<63) |
				numeric_to_int64(low);
		}
		pfree(high_down);
		pfree(high_floor);
		pfree(high_up);
		pfree(low);
#ifdef UNDEBUG
		uint128_log_("numeric_to_uint128() v=%s", v);
#endif
		return v;
	}
	return numeric_to_int64(n);
}

__int128_t
numeric_to_int128(Numeric n)
{
	uint_init_();
#ifdef UNDEBUG
	numeric_log_("numeric_to_int128(%s)", n);
	numeric_log_("numeric_to_int128() zero=%s", zero);
#endif
	if (numeric_lt_(n, zero)) {
		Numeric m = numeric_uminus_(n);
		__uint128_t v = numeric_to_uint128(m);
#ifdef UNDEBUG
		numeric_log_("numeric_to_int128() m=%s", m);
#endif
		pfree(m);
#ifdef UNDEBUG
		uint128_log_("numeric_to_int128() v=%s", v);
#endif
		v = ~v + 1;
#ifdef UNDEBUG
		uint128_log_("numeric_to_int128() v=%s", v);
		int128_log_("numeric_to_int128() (__int128_t)v=%s", v);
#endif
		return v;
	}
	return numeric_to_uint128(n);
}

Numeric
uint64_to_numeric(uint64_t u)
{
	if (unlikely(u & (1ULL<<63))) {
		uint_init_();
		{
			Numeric intermediate = int64_to_numeric(u & ~(1ULL<<63));
			Numeric v = numeric_add_opt_error(intermediate, bit63, NULL);
			pfree(intermediate);
			return v;
		}
	}
	return int64_to_numeric(u);
}

Numeric
uint128_to_numeric(__uint128_t u)
{
	if (unlikely(u >= (((__uint128_t)1)<<64))) {
		uint_init_();
		{
			Numeric high = uint64_to_numeric(u>>64);
			Numeric low = uint64_to_numeric(u);
			Numeric intermediate = numeric_mul_opt_error(high, bit64, NULL);
			Numeric v = numeric_add_opt_error(intermediate, low, NULL);
			pfree(high);
			pfree(low);
			pfree(intermediate);
			return v;
		}
	}
	return uint64_to_numeric(u);
}

Numeric
int128_to_numeric(__int128_t u_)
{
	__uint128_t u = u_;
	if (u_ < 0) {
		Numeric intermediate = uint128_to_numeric(~u + 1);
		Numeric v = numeric_uminus_(intermediate);
		pfree(intermediate);
		return v;
	}
	return uint128_to_numeric(u);
}
