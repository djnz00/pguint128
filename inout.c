#include <ctype.h>

#include <postgres.h>
#include <fmgr.h>
#include <libpq/pqformat.h>
#include <utils/builtins.h>

#include "uint.h"
#include "ntoa.h"

/* #include <inttypes.h> */
#include <limits.h>

/*
 * Copy of old pg_atoi() from PostgreSQL, cut down to support int8 only.
 */
static int8
my_pg_atoi8(const char *s)
{
	long		result;
	char	   *badp;

	/*
	 * Some versions of strtol treat the empty string as an error, but some
	 * seem not to.  Make an explicit test to be sure we catch it.
	 */
	if (s == NULL)
		elog(ERROR, "NULL pointer");
	if (*s == 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"integer", s)));

	errno = 0;
	result = strtol(s, &badp, 10);

	/* We made no progress parsing the string, so bail out */
	if (s == badp)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"integer", s)));

	if (errno == ERANGE || result < SCHAR_MIN || result > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("value \"%s\" is out of range for 8-bit integer", s)));

	/*
	 * Skip any trailing whitespace; if anything but whitespace remains before
	 * the terminating character, bail out
	 */
	while (*badp && isspace((unsigned char) *badp))
		badp++;

	if (*badp)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"integer", s)));

	return (int8) result;
}

PG_FUNCTION_INFO_V1(int1in);
Datum
int1in(PG_FUNCTION_ARGS)
{
	char	   *s = PG_GETARG_CSTRING(0);

	PG_RETURN_INT8(my_pg_atoi8(s));
}

PG_FUNCTION_INFO_V1(int1out);
Datum
int1out(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	char	   *result = palloc(5);		/* sign, 3 digits, '\0' */

	itoa8(result, arg1);
	/* sprintf(result, "%d", arg1); */
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(int1recv);
Datum
int1recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo)PG_GETARG_POINTER(0);

	PG_RETURN_INT8((int8) pq_getmsgint(buf, sizeof(int8)));
}

PG_FUNCTION_INFO_V1(int1send);
Datum
int1send(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint8(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

static uint32
pg_atou(const char *s, int size)
{
	unsigned long int result;
	bool		out_of_range = false;
	char	   *badp;

	if (s == NULL)
		elog(ERROR, "NULL pointer");
	if (*s == 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	if (strchr(s, '-'))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	errno = 0;
	result = strtoul(s, &badp, 10);

	switch (size)
	{
		case sizeof(uint32):
			if (errno == ERANGE
#if defined(HAVE_LONG_INT_64)
				|| result > UINT_MAX
				#endif
				)
				out_of_range = true;
			break;
		case sizeof(uint16):
			if (errno == ERANGE || result > USHRT_MAX)
				out_of_range = true;
			break;
		case sizeof(uint8):
			if (errno == ERANGE || result > UCHAR_MAX)
				out_of_range = true;
			break;
		default:
			elog(ERROR, "unsupported result size: %d", size);
	}

	if (out_of_range)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("value \"%s\" is out of range for type uint%d", s, size)));

	while (*badp && isspace((unsigned char) *badp))
		badp++;

	if (*badp)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	return result;
}

PG_FUNCTION_INFO_V1(uint1in);
Datum
uint1in(PG_FUNCTION_ARGS)
{
	char	   *s = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT8(pg_atou(s, sizeof(uint8)));
}

PG_FUNCTION_INFO_V1(uint1out);
Datum
uint1out(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	char	   *result = palloc(4);		/* 3 digits, '\0' */

	utoa8(result, arg1);
	/* sprintf(result, "%u", arg1); */
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uint1recv);
Datum
uint1recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo)PG_GETARG_POINTER(0);

	PG_RETURN_UINT8((uint8) pq_getmsgint(buf, sizeof(uint8)));
}

PG_FUNCTION_INFO_V1(uint1send);
Datum
uint1send(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint8(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(uint2in);
Datum
uint2in(PG_FUNCTION_ARGS)
{
	char	   *s = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT16(pg_atou(s, sizeof(uint16)));
}

PG_FUNCTION_INFO_V1(uint2out);
Datum
uint2out(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	char	   *result = palloc(6);		/* 5 digits, '\0' */

	utoa32(result, arg1);
	/* sprintf(result, "%u", arg1); */
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uint2recv);
Datum
uint2recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo)PG_GETARG_POINTER(0);

	PG_RETURN_UINT16((uint16) pq_getmsgint(buf, sizeof(uint16)));
}

PG_FUNCTION_INFO_V1(uint2send);
Datum
uint2send(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT16(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint16(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(uint4in);
Datum
uint4in(PG_FUNCTION_ARGS)
{
	char	   *s = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT32(pg_atou(s, sizeof(uint32)));
}

PG_FUNCTION_INFO_V1(uint4out);
Datum
uint4out(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	char	   *result = palloc(11);	/* 10 digits, '\0' */

	utoa32(result, arg1);
	/* sprintf(result, "%u", arg1); */
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uint4recv);
Datum
uint4recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo)PG_GETARG_POINTER(0);

	PG_RETURN_UINT32((uint32) pq_getmsgint(buf, sizeof(uint32)));
}

PG_FUNCTION_INFO_V1(uint4send);
Datum
uint4send(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT32(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint32(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(uint8in);
Datum
uint8in(PG_FUNCTION_ARGS)
{
	char	   *s = PG_GETARG_CSTRING(0);
	unsigned long long int result;
	char	   *badp;

	if (s == NULL)
		elog(ERROR, "NULL pointer");
	if (*s == 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	if (strchr(s, '-'))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	errno = 0;
	result = strtoull(s, &badp, 10);

	if (errno == ERANGE)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("value \"%s\" is out of range for type uint%d", s, 8)));

	while (*badp && isspace((unsigned char) *badp))
		badp++;

	if (*badp)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for unsigned integer: \"%s\"",
						s)));

	PG_RETURN_UINT64(result);
}

PG_FUNCTION_INFO_V1(uint8out);
Datum
uint8out(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	char	   *result = palloc(21);	/* 20 digits, '\0' */

	utoa64(result, arg1);
	/* sprintf(result, "%"PRIu64, (uint64_t) arg1); */
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uint8recv);
Datum
uint8recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo)PG_GETARG_POINTER(0);

	PG_RETURN_UINT64((uint64) pq_getmsgint64(buf));
}

PG_FUNCTION_INFO_V1(uint8send);
Datum
uint8send(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT64(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

static unsigned int
atou128(const char *s, __uint128_t *r)
{
    int c = s[0];
	__uint128_t v;
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
atoi128(const char *s, __int128_t *r)
{
	if (s[0] == '-') {
		unsigned int o = atou128(&s[1], (__uint128_t *)r);
		if (!o) return 0;
		*r = -*r;
		return o + 1;
	}
	return atou128(s, (__uint128_t *)r);
}

PG_FUNCTION_INFO_V1(int16in);
Datum
int16in(PG_FUNCTION_ARGS)
{
	const char *s = PG_GETARG_CSTRING(0);
	__int128_t i;
	unsigned int n = atoi128(s, &i);

	/* SQL requires trailing spaces to be ignored while erroring out on other
	 * "trailing junk" */
	if (likely(n)) while (unlikely(isspace(s[n]))) ++n;
	if (!n || s[n])
		ereport(
			ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			 errmsg("invalid input syntax for type int16: \"%s\"", s)));

	{
		xint128 *v = (xint128 *)palloc(sizeof(xint128));
		v->i = i;
		PG_RETURN_POINTER(v);
	}
}

PG_FUNCTION_INFO_V1(int16out);
Datum
int16out(PG_FUNCTION_ARGS)
{
	xint128			*arg1 = (xint128 *)PG_GETARG_POINTER(0);
	char			*result = palloc(41);	/* sign, 39 digits, '\0' */
	itoa128(result, arg1->i);
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uint16in);
Datum
uint16in(PG_FUNCTION_ARGS)
{
	__uint128_t i;
	const char *s = PG_GETARG_CSTRING(0);
	unsigned int n = atou128(s, &i);

	/* SQL requires trailing spaces to be ignored while erroring out on other
	 * "trailing junk" */
	if (likely(n)) while (unlikely(isspace(s[n]))) ++n;
	if (!n || s[n])
		ereport(
			ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			 errmsg("invalid input syntax for type uint16: \"%s\"", s)));

	{
	  xuint128 *v = (xuint128 *)palloc(sizeof(xuint128));
	  v->i = i;
	  PG_RETURN_POINTER(v);
	}
}

PG_FUNCTION_INFO_V1(uint16out);
Datum
uint16out(PG_FUNCTION_ARGS)
{
	xuint128		*arg1 = (xuint128 *)PG_GETARG_POINTER(0);
	char			*result = palloc(40);	/* 39 digits, '\0' */
	utoa128(result, arg1->i);
	PG_RETURN_CSTRING(result);
}

#include <port/pg_bswap.h>
#ifndef pg_bswap128
#ifdef WORDS_BIGENDIAN
#define pg_bswap128(x) (x)
#else
#if defined(__GNUC__) && !defined(__llvm__)
#define pg_bswap128(x) __builtin_bswap128(x)
#else
inline static __uint128_t
pg_bswap128(__uint128_t i)
{
	return
		((__uint128_t)(pg_bswap64(i))<<64) |
		(__uint128_t)pg_bswap64(i>>64);
}
#endif /* __GNUC__ && !__llvm__ */
#endif /* WORDS_BIGENDIAN */
#endif /* pg_bswap128 */

PG_FUNCTION_INFO_V1(int16recv);
Datum
int16recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
	xint128 *v = (xint128 *)palloc(sizeof(xint128));
	pq_copymsgbytes(buf, (char *)v, 16);
	v->i = (__int128_t)pg_bswap128(v->i);
	PG_RETURN_POINTER(v);
}

PG_FUNCTION_INFO_V1(int16send);
Datum
int16send(PG_FUNCTION_ARGS)
{
	xint128 *v = (xint128 *)PG_GETARG_POINTER(0);
	StringInfoData buf;
	pq_begintypsend(&buf);
	enlargeStringInfo(&buf, 16);
	Assert(buf.len + 16 <= buf.maxlen);
	v->i = (__int128_t)pg_bswap128(v->i);
	memcpy((char *)(buf.data + buf.len), v, 16);
	buf.len += 16;
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(uint16recv);
Datum
uint16recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
	xuint128 *v = (xuint128 *)palloc(sizeof(xuint128));
	pq_copymsgbytes(buf, (char *)v, 16);
	v->i = pg_bswap128(v->i);
	PG_RETURN_POINTER(v);
}

PG_FUNCTION_INFO_V1(uint16send);
Datum
uint16send(PG_FUNCTION_ARGS)
{
	xuint128 *v = (xuint128 *)PG_GETARG_POINTER(0);
	StringInfoData buf;
	pq_begintypsend(&buf);
	enlargeStringInfo(&buf, 16);
	Assert(buf.len + 16 <= buf.maxlen);
	v->i = (__int128_t)pg_bswap128(v->i);
	memcpy((char *)(buf.data + buf.len), v, 16);
	buf.len += 16;
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}
