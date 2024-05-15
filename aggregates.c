#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>

#include "uint.h"
#include "unumeric.h"

#define make_sum_func(argtype, ARGTYPE, RETTYPE) \
PG_FUNCTION_INFO_V1(argtype##_sum); \
Datum \
argtype##_sum(PG_FUNCTION_ARGS) \
{ \
	if (PG_ARGISNULL(0) && PG_ARGISNULL(1)) \
		PG_RETURN_NULL(); \
\
	PG_RETURN_##RETTYPE((PG_ARGISNULL(0) ? 0 : PG_GETARG_##RETTYPE(0)) + \
						(PG_ARGISNULL(1) ? 0 : PG_GETARG_##ARGTYPE(1))); \
} \
extern int no_such_variable

make_sum_func(int1, INT8, INT32);
make_sum_func(uint1, UINT8, UINT32);
make_sum_func(uint2, UINT16, UINT64);
make_sum_func(uint4, UINT32, UINT64);
make_sum_func(uint8, UINT64, UINT64);


typedef struct Int8TransTypeData
{
	int64		count;
	int64		sum;
} Int8TransTypeData;

#define make_avg_func(argtype, ARGTYPE) \
PG_FUNCTION_INFO_V1(argtype##_avg_accum); \
Datum \
argtype##_avg_accum(PG_FUNCTION_ARGS) \
{ \
	ArrayType  *transarray; \
	Int8TransTypeData *transdata; \
\
	transarray = (AggCheckCallContext(fcinfo, NULL)) \
		? PG_GETARG_ARRAYTYPE_P(0) \
		: PG_GETARG_ARRAYTYPE_P_COPY(0); \
\
	if (ARR_HASNULL(transarray) || \
		ARR_SIZE(transarray) != ARR_OVERHEAD_NONULLS(1) + sizeof(Int8TransTypeData)) \
		elog(ERROR, "expected 2-element int8 array"); \
\
	transdata = (Int8TransTypeData *) ARR_DATA_PTR(transarray); \
	transdata->count++; \
	transdata->sum += PG_GETARG_##ARGTYPE(1); \
\
	PG_RETURN_ARRAYTYPE_P(transarray); \
} \
extern int no_such_variable

make_avg_func(int1, INT8);
make_avg_func(uint1, UINT8);
make_avg_func(uint2, UINT16);
make_avg_func(uint4, UINT32);

PG_FUNCTION_INFO_V1(int16_sum);
Datum
int16_sum(PG_FUNCTION_ARGS)
{
	if (unlikely(PG_ARGISNULL(0))) {
		if (unlikely(PG_ARGISNULL(1))) PG_RETURN_NULL();
		PG_RETURN_POINTER(PG_GETARG_POINTER(1));
	}
	if (unlikely(PG_ARGISNULL(1))) PG_RETURN_POINTER(PG_GETARG_POINTER(0));
	{
		xint128 *l = (xint128 *)PG_GETARG_POINTER(0);
		xint128 *r = (xint128 *)PG_GETARG_POINTER(1);
		if (AggCheckCallContext(fcinfo, NULL)) {
		  l->i += r->i;
		  PG_RETURN_POINTER(l);
		} else {
		  xint128 *v = (xint128 *)palloc(sizeof(xint128));
		  v->i = l->i + r->i;
		  PG_RETURN_POINTER(v);
		}
	}
}

PG_FUNCTION_INFO_V1(uint16_sum);
Datum
uint16_sum(PG_FUNCTION_ARGS)
{
	if (unlikely(PG_ARGISNULL(0))) {
		if (unlikely(PG_ARGISNULL(1))) PG_RETURN_NULL();
		PG_RETURN_POINTER(PG_GETARG_POINTER(1));
	}
	if (unlikely(PG_ARGISNULL(1))) PG_RETURN_POINTER(PG_GETARG_POINTER(0));
	{
		xuint128 *l = (xuint128 *)PG_GETARG_POINTER(0);
		xuint128 *r = (xuint128 *)PG_GETARG_POINTER(1);
		if (AggCheckCallContext(fcinfo, NULL)) {
		  l->i += r->i;
		  PG_RETURN_POINTER(l);
		} else {
		  xuint128 *v = (xuint128 *)palloc(sizeof(xuint128));
		  v->i = l->i + r->i;
		  PG_RETURN_POINTER(v);
		}
	}
}

/* missing */
#ifndef PG_GETARG_UINT64
#define PG_GETARG_UINT64(n)  DatumGetUInt64(PG_GETARG_DATUM(n))
#endif

PG_FUNCTION_INFO_V1(uint8_avg_accum);
Datum uint8_avg_accum(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	uint64 *state;
	uint64 v;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(uint64) * 2) {
		elog(ERROR, "uint8_avg_accum expected 2-element uint8 array");
		PG_RETURN_ARRAYTYPE_P(array);
	}

	if (PG_ARGISNULL(1)) PG_RETURN_ARRAYTYPE_P(array);

	state = (uint64 *)ARR_DATA_PTR(array);
	v = PG_GETARG_UINT64(1);

	state[0] += v;
	++state[1];

	PG_RETURN_ARRAYTYPE_P(array);
}

PG_FUNCTION_INFO_V1(uint8_avg);
Datum uint8_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const uint64 *state;
	Numeric sum, count, result;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(uint64) * 2) {
		elog(ERROR, "uint8_avg expected 2-element uint8 array");
		PG_RETURN_NULL();
	}

	state = (const uint64 *)ARR_DATA_PTR(array);

	if (unlikely(!state[1])) PG_RETURN_NULL();

	sum = uint64_to_numeric(state[0]);
	count = uint64_to_numeric(state[1]);
	result = numeric_div_opt_error(sum, count, NULL);
	pfree(sum);
	pfree(count);

	PG_RETURN_DATUM(NumericGetDatum(result));
}

PG_FUNCTION_INFO_V1(int16_avg_accum);
Datum int16_avg_accum(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	xint128 *state;
	xint128 *v;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(xint128) * 2) {
		elog(ERROR, "int16_avg_accum expected 2-element int16 array");
		PG_RETURN_ARRAYTYPE_P(array);
	}

	if (PG_ARGISNULL(1)) PG_RETURN_ARRAYTYPE_P(array);

	state = (xint128 *)ARR_DATA_PTR(array);
	v = (xint128 *)PG_GETARG_POINTER(1);

	state[0].i += v->i;
	++state[1].i;

	PG_RETURN_ARRAYTYPE_P(array);
}

PG_FUNCTION_INFO_V1(int16_avg);
Datum int16_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const xint128 *state;
	Numeric sum, count, result;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(xint128) * 2) {
		elog(ERROR, "int16_avg expected 2-element int16 array");
		PG_RETURN_NULL();
	}

	state = (const xint128 *)ARR_DATA_PTR(array);

	if (unlikely(!state[1].i)) PG_RETURN_NULL();

	sum = int128_to_numeric(state[0].i);
	count = int128_to_numeric(state[1].i);
	result = numeric_div_opt_error(sum, count, NULL);
	pfree(sum);
	pfree(count);

	PG_RETURN_DATUM(NumericGetDatum(result));
}

PG_FUNCTION_INFO_V1(uint16_avg_accum);
Datum uint16_avg_accum(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	xuint128 *state;
	xuint128 *v;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(xuint128) * 2) {
		elog(ERROR, "uint16_avg_accum expected 2-element uint16 array");
		PG_RETURN_ARRAYTYPE_P(array);
	}

	if (PG_ARGISNULL(1)) PG_RETURN_ARRAYTYPE_P(array);

	state = (xuint128 *)ARR_DATA_PTR(array);
	v = (xuint128 *)PG_GETARG_POINTER(1);

	state[0].i += v->i;
	++state[1].i;

	PG_RETURN_ARRAYTYPE_P(array);
}

PG_FUNCTION_INFO_V1(uint16_avg);
Datum uint16_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const xuint128 *state;
	Numeric sum, count, result;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(xuint128) * 2) {
		elog(ERROR, "uint16_avg expected 2-element uint16 array");
		PG_RETURN_NULL();
	}

	state = (const xuint128 *)ARR_DATA_PTR(array);

	if (unlikely(!state[1].i)) PG_RETURN_NULL();

	sum = uint128_to_numeric(state[0].i);
	count = uint128_to_numeric(state[1].i);
	result = numeric_div_opt_error(sum, count, NULL);
	pfree(sum);
	pfree(count);

	PG_RETURN_DATUM(NumericGetDatum(result));
}
