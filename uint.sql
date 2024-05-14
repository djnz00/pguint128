CREATE TYPE int1;

CREATE FUNCTION int1in(cstring) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1in';

CREATE FUNCTION int1out(int1) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1out';

CREATE FUNCTION int1recv(internal) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1recv';

CREATE FUNCTION int1send(int1) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1send';

CREATE TYPE int1 (
    INPUT = int1in,
    OUTPUT = int1out,
    RECEIVE = int1recv,
    SEND = int1send,
    INTERNALLENGTH = 1,
    PASSEDBYVALUE,
    ALIGNMENT = char
);

CREATE FUNCTION int1_to_double(int1) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_to_double';

CREATE FUNCTION int1_to_numeric(int1) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_to_numeric';

CREATE FUNCTION int1_to_real(int1) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_to_real';

CREATE FUNCTION int1_from_double(double precision) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_from_double';

CREATE FUNCTION int1_from_numeric(numeric) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_from_numeric';

CREATE FUNCTION int1_from_real(real) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1_from_real';

CREATE CAST (double precision AS int1)
	WITH FUNCTION int1_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS int1) WITH FUNCTION int1_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS int1) WITH FUNCTION int1_from_real AS ASSIGNMENT;

CREATE CAST (int1 AS double precision)
	WITH FUNCTION int1_to_double AS IMPLICIT;
CREATE CAST (int1 AS numeric) WITH FUNCTION int1_to_numeric AS IMPLICIT;
CREATE CAST (int1 AS real) WITH FUNCTION int1_to_real AS IMPLICIT;


CREATE TYPE uint1;

CREATE FUNCTION uint1in(cstring) RETURNS uint1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1in';

CREATE FUNCTION uint1out(uint1) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1out';

CREATE FUNCTION uint1recv(internal) RETURNS uint1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1recv';

CREATE FUNCTION uint1send(uint1) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1send';

CREATE TYPE uint1 (
    INPUT = uint1in,
    OUTPUT = uint1out,
    RECEIVE = uint1recv,
    SEND = uint1send,
    INTERNALLENGTH = 1,
    PASSEDBYVALUE,
    ALIGNMENT = char
);

CREATE FUNCTION uint1_to_double(uint1) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_to_double';

CREATE FUNCTION uint1_to_numeric(uint1) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_to_numeric';

CREATE FUNCTION uint1_to_real(uint1) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_to_real';

CREATE FUNCTION uint1_from_double(double precision) RETURNS uint1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_from_double';

CREATE FUNCTION uint1_from_numeric(numeric) RETURNS uint1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_from_numeric';

CREATE FUNCTION uint1_from_real(real) RETURNS uint1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint1_from_real';

CREATE CAST (double precision AS uint1)
	WITH FUNCTION uint1_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS uint1) WITH FUNCTION uint1_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS uint1) WITH FUNCTION uint1_from_real AS ASSIGNMENT;

CREATE CAST (uint1 AS double precision)
	WITH FUNCTION uint1_to_double AS IMPLICIT;
CREATE CAST (uint1 AS numeric) WITH FUNCTION uint1_to_numeric AS IMPLICIT;
CREATE CAST (uint1 AS real) WITH FUNCTION uint1_to_real AS IMPLICIT;


CREATE TYPE uint2;

CREATE FUNCTION uint2in(cstring) RETURNS uint2
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2in';

CREATE FUNCTION uint2out(uint2) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2out';

CREATE FUNCTION uint2recv(internal) RETURNS uint2
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2recv';

CREATE FUNCTION uint2send(uint2) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2send';

CREATE TYPE uint2 (
    INPUT = uint2in,
    OUTPUT = uint2out,
    RECEIVE = uint2recv,
    SEND = uint2send,
    INTERNALLENGTH = 2,
    PASSEDBYVALUE,
    ALIGNMENT = int2
);

CREATE FUNCTION uint2_to_double(uint2) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_to_double';

CREATE FUNCTION uint2_to_numeric(uint2) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_to_numeric';

CREATE FUNCTION uint2_to_real(uint2) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_to_real';

CREATE FUNCTION uint2_from_double(double precision) RETURNS uint2
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_from_double';

CREATE FUNCTION uint2_from_numeric(numeric) RETURNS uint2
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_from_numeric';

CREATE FUNCTION uint2_from_real(real) RETURNS uint2
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint2_from_real';

CREATE CAST (double precision AS uint2)
	WITH FUNCTION uint2_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS uint2) WITH FUNCTION uint2_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS uint2) WITH FUNCTION uint2_from_real AS ASSIGNMENT;

CREATE CAST (uint2 AS double precision)
	WITH FUNCTION uint2_to_double AS IMPLICIT;
CREATE CAST (uint2 AS numeric) WITH FUNCTION uint2_to_numeric AS IMPLICIT;
CREATE CAST (uint2 AS real) WITH FUNCTION uint2_to_real AS IMPLICIT;


CREATE TYPE uint4;

CREATE FUNCTION uint4in(cstring) RETURNS uint4
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4in';

CREATE FUNCTION uint4out(uint4) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4out';

CREATE FUNCTION uint4recv(internal) RETURNS uint4
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4recv';

CREATE FUNCTION uint4send(uint4) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4send';

CREATE TYPE uint4 (
    INPUT = uint4in,
    OUTPUT = uint4out,
    RECEIVE = uint4recv,
    SEND = uint4send,
    INTERNALLENGTH = 4,
    PASSEDBYVALUE,
    ALIGNMENT = int4
);

CREATE FUNCTION uint4_to_double(uint4) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_to_double';

CREATE FUNCTION uint4_to_numeric(uint4) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_to_numeric';

CREATE FUNCTION uint4_to_real(uint4) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_to_real';

CREATE FUNCTION uint4_from_double(double precision) RETURNS uint4
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_from_double';

CREATE FUNCTION uint4_from_numeric(numeric) RETURNS uint4
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_from_numeric';

CREATE FUNCTION uint4_from_real(real) RETURNS uint4
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint4_from_real';

CREATE CAST (double precision AS uint4)
	WITH FUNCTION uint4_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS uint4) WITH FUNCTION uint4_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS uint4) WITH FUNCTION uint4_from_real AS ASSIGNMENT;

CREATE CAST (uint4 AS double precision)
	WITH FUNCTION uint4_to_double AS IMPLICIT;
CREATE CAST (uint4 AS numeric) WITH FUNCTION uint4_to_numeric AS IMPLICIT;
CREATE CAST (uint4 AS real) WITH FUNCTION uint4_to_real AS IMPLICIT;


CREATE TYPE uint8;

CREATE FUNCTION uint8in(cstring) RETURNS uint8
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8in';

CREATE FUNCTION uint8out(uint8) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8out';

CREATE FUNCTION uint8recv(internal) RETURNS uint8
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8recv';

CREATE FUNCTION uint8send(uint8) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8send';

CREATE TYPE uint8 (
    INPUT = uint8in,
    OUTPUT = uint8out,
    RECEIVE = uint8recv,
    SEND = uint8send,
    INTERNALLENGTH = 8,
    PASSEDBYVALUE,  -- requires 64-bit
    ALIGNMENT = double
);

CREATE FUNCTION uint8_to_double(uint8) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_to_double';

CREATE FUNCTION uint8_to_numeric(uint8) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_to_numeric';

CREATE FUNCTION uint8_to_real(uint8) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_to_real';

CREATE FUNCTION uint8_from_double(double precision) RETURNS uint8
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_from_double';

CREATE FUNCTION uint8_from_numeric(numeric) RETURNS uint8
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_from_numeric';

CREATE FUNCTION uint8_from_real(real) RETURNS uint8
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_from_real';

CREATE CAST (double precision AS uint8)
	WITH FUNCTION uint8_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS uint8) WITH FUNCTION uint8_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS uint8) WITH FUNCTION uint8_from_real AS ASSIGNMENT;

CREATE CAST (uint8 AS double precision)
	WITH FUNCTION uint8_to_double AS IMPLICIT;
CREATE CAST (uint8 AS numeric) WITH FUNCTION uint8_to_numeric AS IMPLICIT;
CREATE CAST (uint8 AS real) WITH FUNCTION uint8_to_real AS IMPLICIT;


CREATE TYPE int16;

CREATE FUNCTION int16in(cstring) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16in';

CREATE FUNCTION int16out(int16) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16out';

CREATE FUNCTION int16recv(internal) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16recv';

CREATE FUNCTION int16send(int16) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16send';

CREATE TYPE int16 (
    INPUT = int16in,
    OUTPUT = int16out,
    RECEIVE = int16recv,
    SEND = int16send,
    INTERNALLENGTH = 16,
    ALIGNMENT = char
);

CREATE FUNCTION int16_to_double(int16) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_to_double';

CREATE FUNCTION int16_to_numeric(int16) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_to_numeric';

CREATE FUNCTION int16_to_real(int16) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_to_real';

CREATE FUNCTION int16_from_double(double precision) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_from_double';

CREATE FUNCTION int16_from_numeric(numeric) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_from_numeric';

CREATE FUNCTION int16_from_real(real) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_from_real';

CREATE CAST (double precision AS int16)
	WITH FUNCTION int16_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS int16) WITH FUNCTION int16_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS int16) WITH FUNCTION int16_from_real AS ASSIGNMENT;

CREATE CAST (int16 AS double precision)
	WITH FUNCTION int16_to_double AS IMPLICIT;
CREATE CAST (int16 AS numeric) WITH FUNCTION int16_to_numeric AS IMPLICIT;
CREATE CAST (int16 AS real) WITH FUNCTION int16_to_real AS IMPLICIT;


CREATE TYPE uint16;

CREATE FUNCTION uint16in(cstring) RETURNS uint16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16in';

CREATE FUNCTION uint16out(uint16) RETURNS cstring
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16out';

CREATE FUNCTION uint16recv(internal) RETURNS uint16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16recv';

CREATE FUNCTION uint16send(uint16) RETURNS bytea
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16send';

CREATE TYPE uint16 (
    INPUT = uint16in,
    OUTPUT = uint16out,
    RECEIVE = uint16recv,
    SEND = uint16send,
    INTERNALLENGTH = 16,
    ALIGNMENT = char
);

CREATE FUNCTION uint16_to_double(uint16) RETURNS double precision
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_to_double';

CREATE FUNCTION uint16_to_numeric(uint16) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_to_numeric';

CREATE FUNCTION uint16_to_real(uint16) RETURNS real
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_to_real';

CREATE FUNCTION uint16_from_double(double precision) RETURNS uint16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_from_double';

CREATE FUNCTION uint16_from_numeric(numeric) RETURNS uint16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_from_numeric';

CREATE FUNCTION uint16_from_real(real) RETURNS uint16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_from_real';

CREATE CAST (double precision AS uint16)
	WITH FUNCTION uint16_from_double AS ASSIGNMENT;
CREATE CAST (numeric AS uint16) WITH FUNCTION uint16_from_numeric AS ASSIGNMENT;
CREATE CAST (real AS uint16) WITH FUNCTION uint16_from_real AS ASSIGNMENT;

CREATE CAST (uint16 AS double precision)
	WITH FUNCTION uint16_to_double AS IMPLICIT;
CREATE CAST (uint16 AS numeric) WITH FUNCTION uint16_to_numeric AS IMPLICIT;
CREATE CAST (uint16 AS real) WITH FUNCTION uint16_to_real AS IMPLICIT;


CREATE FUNCTION int1um(int1) RETURNS int1
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int1um';

CREATE OPERATOR - (
   PROCEDURE = int1um,
   RIGHTARG = int1
);

CREATE FUNCTION int16um(int16) RETURNS int16
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16um';

CREATE OPERATOR - (
   PROCEDURE = int16um,
   RIGHTARG = int16
);


CREATE FUNCTION uint8_avg(uint8[]) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint8_avg';

CREATE FUNCTION int16_avg(int16[]) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'int16_avg';

CREATE FUNCTION uint16_avg(uint16[]) RETURNS numeric
    IMMUTABLE STRICT LANGUAGE C
    AS '$libdir/uint', 'uint16_avg';
