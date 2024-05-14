#include <stdio.h>

#include "uint.h"
#include "ntoa.h"

#include <utils/numeric.h>
#include <utils/fmgrprotos.h>

inline static int64_t
numeric_to_int64(Numeric n)
{
	return DatumGetInt64(DirectFunctionCall1(
		numeric_int8, NumericGetDatum(n)));
}

inline static bool
numeric_lt_(Numeric l, Numeric r)
{
	return DatumGetBool(DirectFunctionCall2(
		numeric_lt, NumericGetDatum(l), NumericGetDatum(r)));
}

#if 0
inline static void
numeric_log_(char *fmt, Numeric n)
{
	char *s = DatumGetCString(DirectFunctionCall1(
		numeric_out, NumericGetDatum(n)));
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
	pfree(s);
}

inline static void
int128_log_(char *fmt, __int128_t u)
{
	char s[40];
	itoa128(s, u);
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
}

inline static void
uint128_log_(char *fmt, __uint128_t u)
{
	char s[40];
	utoa128(s, u);
	fprintf(stderr, fmt, s); fputc('\n', stderr); fflush(stderr);
}
#endif

inline static uint64_t
numeric_to_uint64_(Numeric n, Numeric bit63)
{
	/* numeric_log_("numeric_to_uint64_(%s)", n); */
	/* numeric_log_("numeric_to_uint64_() bit63=%s", bit63); */
	if (unlikely(!numeric_lt_(n, bit63))) {
		Numeric high = numeric_div_opt_error(n, bit63, NULL);
		Numeric low = numeric_mod_opt_error(n, bit63, NULL);
		uint64_t v =
			(((uint64_t)numeric_to_int64(high))<<63) |
			numeric_to_int64(low);
		/* numeric_log_("numeric_to_uint64_() high=%s", high); */
		/* numeric_log_("numeric_to_uint64_() low=%s", low); */
		pfree(high);
		pfree(low);
		return v;
	}
	return numeric_to_int64(n);
}

inline static uint64_t
numeric_to_uint64(Numeric n)
{
	Numeric bit62 = int64_to_numeric(1ULL<<62);
	Numeric bit63 = numeric_add_opt_error(bit62, bit62, NULL);
	uint64_t v = numeric_to_uint64_(n, bit63);
	pfree(bit62);
	pfree(bit63);
	return v;
}

inline static __uint128_t
numeric_to_uint128_(Numeric n, Numeric bit63)
{
	/* numeric_log_("numeric_to_uint128_(%s)", n); */
	/* numeric_log_("numeric_to_uint128_() bit63=%s", bit63); */
	if (unlikely(!numeric_lt_(n, bit63))) {
		Numeric high = numeric_div_opt_error(n, bit63, NULL);
		Numeric low = numeric_mod_opt_error(n, bit63, NULL);
		__uint128_t v;
		/* numeric_log_("numeric_to_uint128_() high=%s", high); */
		/* numeric_log_("numeric_to_uint128_() low=%s", low); */
		if (unlikely(!numeric_lt_(high, bit63))) {
			Numeric high_high = numeric_div_opt_error(high, bit63, NULL);
			Numeric high_low = numeric_mod_opt_error(high, bit63, NULL);
			/* numeric_log_("numeric_to_uint128_() high_high=%s", high_high); */
			/* numeric_log_("numeric_to_uint128_() high_low=%s", high_low); */
			v = (((__uint128_t)numeric_to_int64(high_high))<<126) |
				(((__uint128_t)numeric_to_int64(high_low))<<63) |
				numeric_to_int64(low);
			pfree(high_high);
			pfree(high_low);
		} else {
			v = (((__uint128_t)numeric_to_int64(high))<<63) |
				numeric_to_int64(low);
		}
		pfree(high);
		pfree(low);
		/* uint128_log_("numeric_to_uint128_() v=%s", v); */
		return v;
	}
	return numeric_to_int64(n);
}

inline static __uint128_t
numeric_to_uint128(Numeric n)
{
	Numeric bit62 = int64_to_numeric(1ULL<<62);
	Numeric bit63 = numeric_add_opt_error(bit62, bit62, NULL);
	__uint128_t v = numeric_to_uint128_(n, bit63);
	pfree(bit62);
	pfree(bit63);
	return v;
}

inline static Numeric
numeric_neg(Numeric v)
{
	return DatumGetNumeric(DirectFunctionCall1(
		numeric_uminus, NumericGetDatum(v)));
}

inline static __int128_t
numeric_to_int128_(Numeric n, Numeric zero, Numeric bit63)
{
	/* numeric_log_("numeric_to_int128_(%s)", n); */
	if (numeric_lt_(n, zero)) {
		Numeric m = numeric_neg(n);
		__uint128_t v = numeric_to_uint128_(m, bit63);
		/* numeric_log_("numeric_to_int128_() m=%s", m); */
		pfree(m);
		/* uint128_log_("numeric_to_int128_() v=%s", v); */
		v = ~v + 1;
		/* uint128_log_("numeric_to_int128_() v=%s", v); */
		/* int128_log_("numeric_to_int128_() (__int128_t)v=%s", v); */
		return v;
	}
	return numeric_to_uint128_(n, bit63);
}

inline static __int128_t
numeric_to_int128(Numeric n)
{
	Numeric zero = int64_to_numeric(0);
	Numeric bit62 = int64_to_numeric(1ULL<<62);
	Numeric bit63 = numeric_add_opt_error(bit62, bit62, NULL);
	__int128_t v = numeric_to_int128_(n, zero, bit63);
	pfree(zero);
	pfree(bit62);
	pfree(bit63);
	return v;
}

inline static Numeric
uint64_to_numeric_(uint64_t u, Numeric bit63)
{
	if (unlikely(u & (1ULL<<63))) {
		Numeric intermediate = int64_to_numeric(u & ~(1ULL<<63));
		Numeric v = numeric_add_opt_error(intermediate, bit63, NULL);
		pfree(intermediate);
		return v;
	}
	return int64_to_numeric(u);
}

inline static Numeric
uint64_to_numeric(uint64_t u)
{
	if (unlikely(u & (1ULL<<63))) {
		/* double bit62 to get bit63 */
		Numeric bit62 = int64_to_numeric(1ULL<<62);
		Numeric bit63 = numeric_add_opt_error(bit62, bit62, NULL);
		Numeric intermediate = int64_to_numeric(u & ~(1ULL<<63));
		Numeric v = numeric_add_opt_error(intermediate, bit63, NULL);
		pfree(bit62);
		pfree(bit63);
		pfree(intermediate);
		return v;
	}
	return int64_to_numeric(u);
}

inline static Numeric
uint128_to_numeric(__uint128_t u)
{
	if (unlikely(u >= (((__uint128_t)1)<<64))) {
		/* quadruple bit62 to get bit64 */
		Numeric bit62 = int64_to_numeric(1ULL<<62);
		Numeric bit63 = numeric_add_opt_error(bit62, bit62, NULL);
		Numeric bit64 = numeric_add_opt_error(bit63, bit63, NULL);
		Numeric high = uint64_to_numeric_(u>>64, bit63);
		Numeric low = uint64_to_numeric_(u, bit63);
		Numeric intermediate = numeric_mul_opt_error(high, bit64, NULL);
		Numeric v = numeric_add_opt_error(intermediate, low, NULL);
		pfree(bit62);
		pfree(bit63);
		pfree(bit64);
		pfree(high);
		pfree(low);
		pfree(intermediate);
		return v;
	}
	return uint64_to_numeric(u);
}

inline static Numeric
int128_to_numeric(__int128_t u_)
{
	__uint128_t u = u_;
	if (u_ < 0) {
		Numeric intermediate = uint128_to_numeric(~u + 1);
		Numeric v = DatumGetNumeric(
			DirectFunctionCall1(
				numeric_uminus, NumericGetDatum(intermediate)));
		pfree(intermediate);
		return v;
	}
	return uint128_to_numeric(u);
}
