CLEANFILES = $(BUILT_SOURCES) *.core core
#CLEANFILES += *.in
AM_MAKEFLAGS = -s

# consider headers:
#   - include_HEADERS
#   - nobase_include_HEADERS
#   - pkginclude_HEADERS
#   - dist_HEADERS
#   - nodist_HEADERS
#   - noinst_HEADERS
noinst_HEADERS = unittest.h

noinst_PROGRAMS=test-scalar-01 test-scalar-02 test-scalar-03 test-vector-01 test-partial-01 test-partial-02

BUILT_SOURCES = \
	diag.c diag.h \
	data/scalar-01.proto.c data/scalar-01.proto.h \
	data/scalar-02.proto.c data/scalar-02.proto.h \
	data/scalar-03.proto.c data/scalar-03.proto.h \
	data/vector-01.proto.c data/vector-01.proto.h \
	data/partial-01.proto.c data/partial-01.proto.h \
	data/partial-02.proto.c data/partial-02.proto.h

EXTRA_DIST = $(diags) $(data)

#noinst_HEADERS = ../src/mrkprotobuf.h

if DEBUG
DEBUG_FLAGS = -g -O0 @CC_DEBUG@
else
DEBUG_FLAGS = -DNDEBUG -O3
endif

common_cflags = $(DEBUG_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ -I$(top_srcdir)/test -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir) -DPACKAGE_ROOT="\"$(top_srcdir)\""

common_ldflags = -L$(top_srcdir)/src/.libs -lmrkprotobuf -L$(libdir) -lmrkcommon -lmndiag

test_scalar_01_SOURCES = test-scalar-01.c data/scalar-01.proto.c
test_scalar_01_CFLAGS = $(common_cflags)
test_scalar_01_LDFLAGS = $(common_ldflags)

test_scalar_02_SOURCES = test-scalar-02.c data/scalar-02.proto.c
test_scalar_02_CFLAGS = $(common_cflags)
test_scalar_02_LDFLAGS = $(common_ldflags)

test_scalar_03_SOURCES = test-scalar-03.c data/scalar-03.proto.c
test_scalar_03_CFLAGS = $(common_cflags)
test_scalar_03_LDFLAGS = $(common_ldflags)

test_vector_01_SOURCES = test-vector-01.c data/vector-01.proto.c
test_vector_01_CFLAGS = $(common_cflags)
test_vector_01_LDFLAGS = $(common_ldflags)

test_partial_01_SOURCES = test-partial-01.c data/partial-01.proto.c
test_partial_01_CFLAGS = $(common_cflags)
test_partial_01_LDFLAGS = $(common_ldflags)

test_partial_02_SOURCES = test-partial-02.c data/partial-02.proto.c
test_partial_02_CFLAGS = $(common_cflags)
test_partial_02_LDFLAGS = $(common_ldflags)

diags = diag.txt

data = data/*.proto

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L global -H diag.h -C diag.c ../*.[ch] ./*.[ch]

data/scalar-01.proto.c data/scalar-01.proto.h: data/scalar-01.proto
	$(AM_V_GEN) ../src/mrkpbc data/scalar-01.proto

data/scalar-02.proto.c data/scalar-02.proto.h: data/scalar-02.proto
	$(AM_V_GEN) ../src/mrkpbc data/scalar-02.proto

data/scalar-03.proto.c data/scalar-03.proto.h: data/scalar-03.proto
	$(AM_V_GEN) ../src/mrkpbc data/scalar-03.proto

data/vector-01.proto.c data/vector-01.proto.h: data/vector-01.proto
	$(AM_V_GEN) ../src/mrkpbc data/vector-01.proto

data/partial-01.proto.c data/partial-01.proto.h: data/partial-01.proto
	$(AM_V_GEN) ../src/mrkpbc data/partial-01.proto

data/partial-02.proto.c data/partial-02.proto.h: data/partial-02.proto
	$(AM_V_GEN) ../src/mrkpbc data/partial-02.proto

testrun: all
	for i in $(noinst_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;
