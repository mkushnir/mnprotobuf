# consider headers:
#   - include_HEADERS
#   - nobase_include_HEADERS
#   - pkginclude_HEADERS
#   - dist_HEADERS
#   - nodist_HEADERS
#   - noinst_HEADERS

BUILT_SOURCES = mnpbcg.c mnpbcg.h mnpbcl.c diag.c diag.h
EXTRA_DIST = $(diags)
CLEANFILES = $(BUILT_SOURCES) *.core core
#CLEANFILES += *.in
AM_MAKEFLAGS = -s
AM_LIBTOOLFLAGS = --silent

lib_LTLIBRARIES = libmnprotobuf.la

bin_PROGRAMS = mnpbc

nobase_include_HEADERS = mnprotobuf.h

noinst_HEADERS = mnpbc.h mnprotobuf_private.h

libmnprotobuf_la_SOURCES = mnprotobuf.c
nodist_libmnprotobuf_la_SOURCES = diag.c

nodist_mnpbc_SOURCES = diag.c
mnpbc_SOURCES = mnpbcg.y mnpbcg.h mnpbcl.l mnpbcscan.c mnpbcc.c mnpbc-main.c

diags = diag.txt

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
libmnprotobuf_la_LDFLAGS = -all-static
mnpbc_LDFLAGS = -all-static
else
libmnprotobuf_la_LDFLAGS =
mnpbc_LDFLAGS =
endif

libmnprotobuf_la_CFLAGS = $(DEBUG_CC_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ @_XOPEN_SOURCE_MACRO@ -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)

libmnprotobuf_la_LDFLAGS += $(DEBUG_LD_FLAGS) -version-info 0:0:0 -L$(libdir)
libmnprotobuf_la_LIBADD = -lmncommon -lmndiag

mnpbc_CFLAGS = $(DEBUG_CC_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ @_XOPEN_SOURCE_MACRO@ -I$(top_srcdir)/test -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)
mnpbc_LDFLAGS += $(DEBUG_LD_FLAGS) -L$(libdir)
mnpbc_LDADD = -lmncommon -lmndiag $(LEXLIB)

SUBDIRS = .

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L global -H diag.h -C diag.c *.[ch]

run: all
	for i in $(bin_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;

testrun:
	for i in $(SUBDIRS); do if test "$$i" != "."; then cd $$i && $(MAKE) testrun && cd ..; fi; done;
