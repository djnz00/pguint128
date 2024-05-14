PG_CONFIG = pg_config

pg_version := $(word 2,$(shell $(PG_CONFIG) --version))
indexonlyscan_supported = $(filter-out 6.% 7.% 8.% 9.0% 9.1%,$(pg_version))

# Disable index-only scans here so that the regression test output is
# the same in versions that don't support it.
ifneq (,$(indexonlyscan_supported))
export PGOPTIONS = -c enable_indexonlyscan=off
endif

extension_version = 0

EXTENSION = uint
MODULE_big = uint
OBJS = aggregates.o hash.o hex.o inout.o magic.o misc.o operators.o
DATA_built = uint--$(extension_version).sql

REGRESS = init hash hex operators misc drop
REGRESS_OPTS = --inputdir=test

EXTRA_CLEAN += operators.c operators.sql test/sql/operators.sql ntoa_test.o ntoa_test

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

uint--$(extension_version).sql: uint.sql hash.sql hex.sql operators.sql
	cat $^ >$@

PYTHON ?= python

operators.c operators.sql test/sql/operators.sql: generate.py
	$(PYTHON) $< $(MAJORVERSION)

python-check: generate.py
	pep8 $^
	pyflakes $^
	pylint $^

ntoa_test.o: ntoa_test.c
	$(CC) -O3 -g -c ntoa_test.c

ntoa_test: ntoa_test.o
	$(CC) -O3 -g $^ -o $@

ntoa-check: ntoa_test
	./ntoa_test

$(OBJS): uint.h
inout.o: ntoa.h
ntoa_test.o: ntoa.h
misc.o: unumeric.h
aggregates.o: unumeric.h
