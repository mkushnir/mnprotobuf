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

noinst_PROGRAMS=test-scalar-01 test-scalar-02 test-scalar-03 test-scalar-04 test-vector-01 test-partial-01 test-partial-02

BUILT_SOURCES = \
	diag.c diag.h \
	data/scalar-01.c data/scalar-01.h \
	data/scalar-02.c data/scalar-02.h \
	data/scalar-03.c data/scalar-03.h \
	data/scalar-04.c data/scalar-04.h \
	data/vector-01.c data/vector-01.h \
	data/partial-01.c data/partial-01.h \
	data/partial-02.c data/partial-02.h

EXTRA_DIST = $(diags) $(data)

#noinst_HEADERS = ../src/mrkprotobuf.h

DEBUG_LD_FLAGS =
if DEBUG
DEBUG_CC_FLAGS = -g -O0 @CC_DEBUG@
DEBUG_LD_FLAGS += -no-install
else
DEBUG_CC_FLAGS = -DNDEBUG -O3 -fomit-frame-pointer
if LTO
DEBUG_CC_FLAGS += @CC_LTO@
DEBUG_LD_FLAGS += @LD_LTO@
endif
endif

if ALLSTATIC
common_ldflags = -all-static
else
common_ldflags =
endif

common_cflags = $(DEBUG_CC_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ @_XOPEN_SOURCE_MACRO@ -I$(top_srcdir)/test -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir) -DPACKAGE_ROOT="\"$(top_srcdir)\""

common_ldflags += $(DEBUG_LD_FLAGS) -L$(top_srcdir)/src/.libs -L$(libdir)
common_ldadd = -lmrkprotobuf -lmrkcommon -lmndiag

test_scalar_01_SOURCES = test-scalar-01.c data/scalar-01.c
test_scalar_01_CFLAGS = $(common_cflags)
test_scalar_01_LDFLAGS = $(common_ldflags)
test_scalar_01_LDADD = $(common_ldadd)

test_scalar_02_SOURCES = test-scalar-02.c data/scalar-02.c
test_scalar_02_CFLAGS = $(common_cflags)
test_scalar_02_LDFLAGS = $(common_ldflags)
test_scalar_02_LDADD = $(common_ldadd)

test_scalar_03_SOURCES = test-scalar-03.c data/scalar-03.c
test_scalar_03_CFLAGS = $(common_cflags)
test_scalar_03_LDFLAGS = $(common_ldflags)
test_scalar_03_LDADD = $(common_ldadd)

test_scalar_04_SOURCES = test-scalar-04.c data/scalar-04.c
test_scalar_04_CFLAGS = $(common_cflags)
test_scalar_04_LDFLAGS = $(common_ldflags)
test_scalar_04_LDADD = $(common_ldadd)

test_vector_01_SOURCES = test-vector-01.c data/vector-01.c
test_vector_01_CFLAGS = $(common_cflags)
test_vector_01_LDFLAGS = $(common_ldflags)
test_vector_01_LDADD = $(common_ldadd)

test_partial_01_SOURCES = test-partial-01.c data/partial-01.c
test_partial_01_CFLAGS = $(common_cflags)
test_partial_01_LDFLAGS = $(common_ldflags)
test_partial_01_LDADD = $(common_ldadd)

test_partial_02_SOURCES = test-partial-02.c data/partial-02.c
test_partial_02_CFLAGS = $(common_cflags)
test_partial_02_LDFLAGS = $(common_ldflags)
test_partial_02_LDADD = $(common_ldadd)

diags = diag.txt

data = data/*.proto

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L global -H diag.h -C diag.c ../*.[ch] ./*.[ch]

data/scalar-01.c data/scalar-01.h: data/scalar-01.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/scalar-01.h -C data/scalar-01.c data/scalar-01.proto

data/scalar-02.c data/scalar-02.h: data/scalar-02.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/scalar-02.h -C data/scalar-02.c data/scalar-02.proto

data/scalar-03.c data/scalar-03.h: data/scalar-03.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/scalar-03.h -C data/scalar-03.c data/scalar-03.proto

data/scalar-04.c data/scalar-04.h: data/scalar-04.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/scalar-04.h -C data/scalar-04.c data/scalar-04.proto

data/vector-01.c data/vector-01.h: data/vector-01.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/vector-01.h -C data/vector-01.c data/vector-01.proto

data/partial-01.c data/partial-01.h: data/partial-01.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/partial-01.h -C data/partial-01.c data/partial-01.proto

data/partial-02.c data/partial-02.h: data/partial-02.proto
	$(AM_V_GEN) ../src/mrkpbc -H data/partial-02.h -C data/partial-02.c data/partial-02.proto

testrun: all
	for i in $(noinst_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;
