include $(top_srcdir)/Makefile.decl

INCLUDES = \
	-g				\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/xml-reader	\
	$(JSON_DEBUG_CFLAGS)		\
	$(JSON_CFLAGS)

noinst_PROGRAMS = $(TEST_PROGS)
progs_ldadd = $(top_builddir)/xml-reader/libxml-reader-1.0.la

