#include <postgres.h>
#include <fmgr.h>
#include <access/hash.h>

#include "uint.h"

#define make_hashfunc(type, BTYPE, casttype) \
PG_FUNCTION_INFO_V1(hash##type); \
Datum \
hash##type(PG_FUNCTION_ARGS) \
{ \
	return hash_uint32((casttype) PG_GETARG_##BTYPE(0)); \
} \
extern int no_such_variable

make_hashfunc(int1, INT8, int32);
make_hashfunc(uint1, UINT8, uint32);
make_hashfunc(uint2, UINT16, uint32);
make_hashfunc(uint4, UINT32, uint32);

PG_FUNCTION_INFO_V1(hashuint8);
Datum
hashuint8(PG_FUNCTION_ARGS)
{
	/* see also hashint8 */
	uint64		val = PG_GETARG_UINT64(0);
	uint32		lohalf = (uint32) val;
	uint32		hihalf = (uint32) (val >> 32);

	lohalf ^= hihalf;

	return hash_uint32(lohalf);
}

/* carefully crafted, see comments in hashint8 */
PG_FUNCTION_INFO_V1(hashuint16);
Datum
hashuint16(PG_FUNCTION_ARGS)
{
	/* see also hashint8 */
	uint128_t	val = *(uint128_t *)PG_GETARG_POINTER(0);
	uint32		q0 = (uint32) val;
	uint32		q1 = (uint32) (val >> 32);
	uint32		q2 = (uint32) (val >> 64);
	uint32		q3 = (uint32) (val >> 96);

	q2 ^= q3;
	q1 ^= q2;
	q0 ^= q1;

	return hash_uint32(q0);
}
