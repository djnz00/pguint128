#include <utils/numeric.h>

int64_t numeric_to_int64(Numeric n);

uint64_t numeric_to_uint64(Numeric n);

__uint128_t numeric_to_uint128_(Numeric n, Numeric bit63);

__uint128_t numeric_to_uint128(Numeric n);

Numeric numeric_neg(Numeric v);

__int128_t numeric_to_int128_(Numeric n, Numeric zero, Numeric bit63);

__int128_t numeric_to_int128(Numeric n);

Numeric uint64_to_numeric_(uint64_t u, Numeric bit63);

Numeric uint64_to_numeric(uint64_t u);

Numeric uint128_to_numeric(__uint128_t u);

Numeric int128_to_numeric(__int128_t u_);
