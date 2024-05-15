#include <postgres.h>
#include <fmgr.h>

#include "uint.h"
#include "unumeric.h"


PG_FUNCTION_INFO_V1(int1um);
Datum
int1um(PG_FUNCTION_ARGS)
{
	int8		arg = PG_GETARG_INT8(0);
	int8		result;

	result = -arg;
	/* overflow check */
	if (arg != 0 && SAMESIGN(result, arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	PG_RETURN_INT8(result);
}


PG_FUNCTION_INFO_V1(int16um);
Datum
int16um(PG_FUNCTION_ARGS)
{
	xint128 *arg = (xint128 *)PG_GETARG_POINTER(0);
	xint128 *result = (xint128 *)palloc(sizeof(xint128));

	result->i = -(arg->i);
	/* overflow check */
	if (arg != 0 && SAMESIGN((result->i), arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	PG_RETURN_POINTER(result);
}

#define casts(type, ntype, size) \
PG_FUNCTION_INFO_V1(type##_to_double); \
Datum type##_to_double(PG_FUNCTION_ARGS) { \
	PG_RETURN_FLOAT8(PG_GETARG_INT##size(0)); \
} \
 \
PG_FUNCTION_INFO_V1(type##_to_numeric); \
Datum type##_to_numeric(PG_FUNCTION_ARGS) { \
	PG_RETURN_POINTER(ntype##_to_numeric(PG_GETARG_INT##size(0))); \
} \
 \
PG_FUNCTION_INFO_V1(type##_to_real); \
Datum type##_to_real(PG_FUNCTION_ARGS) { \
	PG_RETURN_FLOAT4(PG_GETARG_INT##size(0)); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_double); \
Datum type##_from_double(PG_FUNCTION_ARGS) { \
	PG_RETURN_INT##size(PG_GETARG_FLOAT8(0)); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_numeric); \
Datum type##_from_numeric(PG_FUNCTION_ARGS) { \
	PG_RETURN_INT##size(numeric_to_##ntype((Numeric)PG_GETARG_POINTER(0))); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_real); \
Datum type##_from_real(PG_FUNCTION_ARGS) { \
	PG_RETURN_INT##size(PG_GETARG_FLOAT4(0)); \
}

casts(int1, int64, 8)
casts(uint1, int64, 8)
casts(uint2, int64, 16)
casts(uint4, int64, 32)
casts(uint8, uint64, 64)

#define casts128(type, ntype) \
PG_FUNCTION_INFO_V1(type##_to_double); \
Datum type##_to_double(PG_FUNCTION_ARGS) { \
	PG_RETURN_FLOAT8(((x##ntype *)PG_GETARG_POINTER(0))->i); \
} \
 \
PG_FUNCTION_INFO_V1(type##_to_numeric); \
Datum type##_to_numeric(PG_FUNCTION_ARGS) { \
	const x##ntype *v = (const x##ntype *)PG_GETARG_POINTER(0); \
	PG_RETURN_POINTER(ntype##_to_numeric(v->i)); \
} \
 \
PG_FUNCTION_INFO_V1(type##_to_real); \
Datum type##_to_real(PG_FUNCTION_ARGS) { \
	PG_RETURN_FLOAT4(((x##ntype *)PG_GETARG_POINTER(0))->i); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_double); \
Datum type##_from_double(PG_FUNCTION_ARGS) { \
    x##ntype *v = (x##ntype *)palloc(sizeof(x##ntype)); \
	v->i = PG_GETARG_FLOAT8(0); \
	PG_RETURN_POINTER(v); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_numeric); \
Datum type##_from_numeric(PG_FUNCTION_ARGS) { \
    x##ntype *v = (x##ntype *)palloc(sizeof(x##ntype)); \
	v->i = numeric_to_##ntype((Numeric)PG_GETARG_POINTER(0)); \
	PG_RETURN_POINTER(v); \
} \
 \
PG_FUNCTION_INFO_V1(type##_from_real); \
Datum type##_from_real(PG_FUNCTION_ARGS) { \
    x##ntype *v = (x##ntype *)palloc(sizeof(x##ntype)); \
	v->i = PG_GETARG_FLOAT4(0); \
	PG_RETURN_POINTER(v); \
}

casts128(int16, int128);
casts128(uint16, uint128);
