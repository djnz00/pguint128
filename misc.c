#include <postgres.h>
#include <fmgr.h>

#include "uint.h"


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
