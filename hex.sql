CREATE FUNCTION to_hex(uint4) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/uint', 'to_hex_uint4';
CREATE FUNCTION to_hex(uint8) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/uint', 'to_hex_uint8';
CREATE FUNCTION to_hex(int16) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/uint', 'to_hex_uint16';
CREATE FUNCTION to_hex(uint16) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/uint', 'to_hex_uint16';
