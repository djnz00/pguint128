#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>
#include <utils/numeric.h>
#include <utils/fmgrprotos.h>

#include "uint.h"

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
		int128_t *l = (int128_t *)PG_GETARG_POINTER(0);
		int128_t r = *(int128_t *)PG_GETARG_POINTER(1);
		if (AggCheckCallContext(fcinfo, NULL)) {
		  *l += r;
		  PG_RETURN_POINTER(l);
		} else {
		  int128_t *v = (int128_t *)palloc(sizeof(int128_t));
		  *v = *l + r;
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
		uint128_t *l = (uint128_t *)PG_GETARG_POINTER(0);
		uint128_t r = *(uint128_t *)PG_GETARG_POINTER(1);
		if (AggCheckCallContext(fcinfo, NULL)) {
		  *l += r;
		  PG_RETURN_POINTER(l);
		} else {
		  uint128_t *v = (uint128_t *)palloc(sizeof(uint128_t));
		  *v = *l + r;
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

static Datum
uint64_to_numeric(PG_FUNCTION_ARGS)
{
	uint64_t u = PG_GETARG_UINT64(0);
	Datum v;
	if (unlikely(u & (1ULL<<63))) {
		/* double bit62 to get bit63 */
		Datum bit63 = NumericGetDatum(int64_to_numeric(1ULL<<62));
		bit63 = DirectFunctionCall2(numeric_add, bit63, bit63);
		v = NumericGetDatum(int64_to_numeric(u & ~(1ULL<<63)));
		v = DirectFunctionCall2(numeric_add, v, bit63);
	} else {
		v = NumericGetDatum(int64_to_numeric(u));
	}
	PG_RETURN_DATUM(v);
}

static Datum
uint16_to_numeric(PG_FUNCTION_ARGS)
{
	uint128_t u = *(uint128_t *)PG_GETARG_POINTER(0);
	Datum v;
	if (unlikely(u >= (((uint128_t)1)<<64))) {
		/* quadruple bit62 to get bit64 */
		Datum bit64 = NumericGetDatum(int64_to_numeric(1ULL<<62));
		bit64 = DirectFunctionCall2(numeric_add, bit64, bit64);
		bit64 = DirectFunctionCall2(numeric_add, bit64, bit64);
		v = DirectFunctionCall1(uint64_to_numeric, UInt64GetDatum(u>>64));
		v = DirectFunctionCall2(numeric_mul, v, bit64);
		v = DirectFunctionCall2(
				numeric_add, v,
				DirectFunctionCall1(uint64_to_numeric, UInt64GetDatum(u)));
	} else {
		v = DirectFunctionCall1(uint64_to_numeric, UInt64GetDatum(u));
	}
	PG_RETURN_DATUM(v);
}

static Datum
int16_to_numeric(PG_FUNCTION_ARGS)
{
	int128_t u_ = *(int128_t *)PG_GETARG_POINTER(0);
	uint128_t u = u_;
	Datum v;
	if (u_ < 0) u = ~u + 1;
	v = DirectFunctionCall1(uint16_to_numeric, PointerGetDatum(&u));
	if (u_ < 0) v = DirectFunctionCall1(numeric_uminus, v);
	PG_RETURN_DATUM(v);
}

PG_FUNCTION_INFO_V1(uint8_avg);
Datum uint8_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const uint64 *state;
	Datum sumd, countd;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(uint64) * 2) {
		elog(ERROR, "uint8_avg expected 2-element uint8 array");
		PG_RETURN_NULL();
	}

	state = (const uint64 *)ARR_DATA_PTR(array);

	if (unlikely(!state[1])) PG_RETURN_NULL();

	sumd = DirectFunctionCall1(uint64_to_numeric, UInt64GetDatum(state[0]));
	countd = DirectFunctionCall1(uint64_to_numeric, UInt64GetDatum(state[1]));

	PG_RETURN_DATUM(DirectFunctionCall2(numeric_div, sumd, countd));
}

PG_FUNCTION_INFO_V1(int16_avg_accum);
Datum int16_avg_accum(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	int128_t *state;
	int128_t v;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(int128_t) * 2) {
		elog(ERROR, "int16_avg_accum expected 2-element int16 array");
		PG_RETURN_ARRAYTYPE_P(array);
	}

	if (PG_ARGISNULL(1)) PG_RETURN_ARRAYTYPE_P(array);

	state = (int128_t *)ARR_DATA_PTR(array);
	v = *(const int128_t *)PG_GETARG_POINTER(1);

	state[0] += v;
	++state[1];

	PG_RETURN_ARRAYTYPE_P(array);
}

PG_FUNCTION_INFO_V1(int16_avg);
Datum int16_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const int128_t *state;
	Datum sumd, countd;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(int128_t) * 2) {
		elog(ERROR, "int16_avg expected 2-element int16 array");
		PG_RETURN_NULL();
	}

	state = (const int128_t *)ARR_DATA_PTR(array);

	if (unlikely(!state[1])) PG_RETURN_NULL();

	sumd = DirectFunctionCall1(
			uint16_to_numeric, PointerGetDatum(&state[0]));
	countd = DirectFunctionCall1(
			uint16_to_numeric, PointerGetDatum(&state[1]));

	PG_RETURN_DATUM(DirectFunctionCall2(numeric_div, sumd, countd));
}

PG_FUNCTION_INFO_V1(uint16_avg_accum);
Datum uint16_avg_accum(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	uint128_t *state;
	uint128_t v;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(uint128_t) * 2) {
		elog(ERROR, "uint16_avg_accum expected 2-element uint16 array");
		PG_RETURN_ARRAYTYPE_P(array);
	}

	if (PG_ARGISNULL(1)) PG_RETURN_ARRAYTYPE_P(array);

	state = (uint128_t *)ARR_DATA_PTR(array);
	v = *(const uint128_t *)PG_GETARG_POINTER(1);

	state[0] += v;
	++state[1];

	PG_RETURN_ARRAYTYPE_P(array);
}

PG_FUNCTION_INFO_V1(uint16_avg);
Datum uint16_avg(PG_FUNCTION_ARGS) {
	ArrayType *array = AggCheckCallContext(fcinfo, NULL) ?
		PG_GETARG_ARRAYTYPE_P(0) :
		PG_GETARG_ARRAYTYPE_P_COPY(0);
	const uint128_t *state;
	Datum sumd, countd;

	if (ARR_NDIM(array) != 1 ||
		ARR_DIMS(array)[0] != 2 ||
		ARR_HASNULL(array) ||
		ARR_SIZE(array) != ARR_OVERHEAD_NONULLS(1) + sizeof(uint128_t) * 2) {
		elog(ERROR, "uint16_avg expected 2-element uint16 array");
		PG_RETURN_NULL();
	}

	state = (const uint128_t *)ARR_DATA_PTR(array);

	if (unlikely(!state[1])) PG_RETURN_NULL();

	sumd = DirectFunctionCall1(
			int16_to_numeric, PointerGetDatum(&state[0]));
	countd = DirectFunctionCall1(
			int16_to_numeric, PointerGetDatum(&state[1]));

	PG_RETURN_DATUM(DirectFunctionCall2(numeric_div, sumd, countd));
}
