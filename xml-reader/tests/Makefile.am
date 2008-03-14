include $(top_srcdir)/Makefile.decl

INCLUDES = \
	-g				\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/xml-reader	\
	$(XMLR_DEBUG_CFLAGS)		\
	$(XMLR_CFLAGS)

noinst_PROGRAMS = $(TEST_PROGS)
progs_ldadd = $(top_builddir)/xml-reader/libxml-reader-1.0.la

TEST_ENVIRONMENT = srcdir=$(srcdir)

TEST_PROGS          += test-reader
test_reader_SOURCES  = test-reader.c
test_reader_LDADD    = $(progs_ldadd)

