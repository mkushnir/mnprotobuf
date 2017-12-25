# consider headers:
#   - include_HEADERS
#   - nobase_include_HEADERS
#   - pkginclude_HEADERS
#   - dist_HEADERS
#   - nodist_HEADERS
#   - noinst_HEADERS

BUILT_SOURCES = mrkpbcg.c mrkpbcg.h mrkpbcl.c diag.c diag.h
EXTRA_DIST = $(diags)
CLEANFILES = $(BUILT_SOURCES) *.core core
#CLEANFILES += *.in
AM_MAKEFLAGS = -s

lib_LTLIBRARIES = libmrkprotobuf.la

bin_PROGRAMS = mrkpbc

nobase_include_HEADERS = mrkprotobuf.h

noinst_HEADERS = mrkpbc.h

libmrkprotobuf_la_SOURCES = mrkprotobuf.c
nodist_libmrkprotobuf_la_SOURCES = diag.c

nodist_mrkpbc_SOURCES = diag.c
mrkpbc_SOURCES = mrkpbcg.y mrkpbcl.l mrkpbcscan.c mrkpbcc.c mrkpbc-main.c

diags = diag.txt

if DEBUG
DEBUG_FLAGS = -g -O0 @CC_DEBUG@
else
DEBUG_FLAGS = -DNDEBUG -O3
endif

libmrkprotobuf_la_CFLAGS = $(DEBUG_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)

#libmrkprotobuf_la_LDFLAGS = -version-info 0:0:0
libmrkprotobuf_la_LDFLAGS = -version-info 0:0:0 -L$(libdir) -lmrkcommon
#libmrkprotobuf_la_LDFLAGS = -all-static
#libmrkprotobuf_la_LDFLAGS = -all-static -Wl,-Bdynamic,-L$(libdir),-lfoo -lqwe,-Bstatic

mrkpbc_CFLAGS = $(DEBUG_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ -I$(top_srcdir)/test -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)
#mrkpbc_LDFLAGS = -L$(libdir) -lmrkcommon -L$(top_srcdir)/src/.libs -lmrkprotobuf
mrkpbc_LDFLAGS = -L$(libdir) -lmrkcommon -lmndiag -lfl

SUBDIRS = .

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L global -H diag.h -C diag.c *.[ch]

run: all
	for i in $(bin_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;

testrun:
	for i in $(SUBDIRS); do if test "$$i" != "."; then cd $$i && $(MAKE) testrun && cd ..; fi; done;
